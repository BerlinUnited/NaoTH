#
# state.rb
#
# Copyright (c) 1999-2003 Minero Aoki <aamine@loveruby.net>
#
# This program is free software.
# You can distribute/modify this program under the terms of
# the GNU LGPL, Lesser General Public License version 2.
# For details of the GNU LGPL, see the file "COPYING".
#

require 'racc/iset'


module Racc

  class RaccError < StandardError; end

  #
  # StateTable
  #
  # the table of lalr states.
  #

  class StateTable

    def initialize( racc )
      @ruletable   = racc.ruletable
      @symboltable = racc.symboltable

      @d_state = racc.d_state
      @d_la    = racc.d_la
      @d_prec  = racc.d_prec

      @states = []
      @statecache = {}

      @actions = ActionTable.new(@ruletable, self)
    end

    attr_reader :actions

    def size
      @states.size
    end

    def inspect
      '#<state table>'
    end

    alias to_s inspect

    def []( i )
      @states[i]
    end

    def each_state( &block )
      @states.each(&block)
    end

    alias each each_state

    def each_index( &block )
      @states.each_index(&block)
    end

    ###
    ### nfa
    ###

    def init
      # add state 0
      core_to_state  [ @ruletable[0].ptrs[0] ]

      cur = 0
      @gotos = []
      while cur < @states.size
        generate_states @states[cur]   # state is added here
        cur += 1
      end

      @actions.init
    end

    def generate_states( state )
      puts "dstate: #{state}" if @d_state

      table = {}
      ptr = pt = s = g = nil

      state.closure.each do |ptr|
        if sym = ptr.dereference
          addsym table, sym, ptr.next
        end
      end

      table.each do |sym, core|
        puts "dstate: sym=#{sym} ncore=#{core}" if @d_state

        dest = core_to_state(core.to_a)
        state.goto_table[sym] = dest
        id = sym.nonterminal?() ? @gotos.size : nil
        g = Goto.new(id, sym, state, dest)
        @gotos.push g if sym.nonterminal?
        state.gotos[sym] = g
        puts "dstate: #{state.ident} --#{sym}--> #{dest.ident}" if @d_state

        # check infinite recursion
        if state.ident == dest.ident and state.closure.size == 1
          raise RaccError, sprintf("Infinite recursion: state %d, with rule %d",
                                   state.ident, state.ptrs[0].rule.ident)
        end
      end
    end

    def addsym( table, sym, ptr )
      unless s = table[sym]
        table[sym] = s = ISet.new
      end
      s.add ptr
    end

    def core_to_state( core )
      #
      # convert CORE to a State object.
      # If matching state does not exist, create it and add to the table.
      #

      k = fingerprint(core)
      unless dest = @statecache[k]
        # not registered yet
        dest = State.new(@states.size, core)
        @states.push dest

        @statecache[k] = dest
        
        puts "core_to_state: create state   ID #{dest.ident}" if @d_state
      else
        if @d_state
          puts "core_to_state: dest is cached ID #{dest.ident}"
          puts "core_to_state: dest core #{dest.core.join(' ')}"
        end
      end

      dest
    end

    def fingerprint( arr )
      arr.map {|i| i.ident }.pack('L*')
    end

    ###
    ### dfa
    ###

    def determine
      la = lookahead
      @states.each do |state|
        state.set_la la
        resolve state
      end

      set_accept
      @states.each do |state|
        pack state
      end
      check_useless
    end

    #
    # lookahead
    #

    def lookahead
      #
      # lookahead algorithm ver.3 -- from bison 1.26
      #
      state = goto = arr = ptr = st = rl = i = a = t = g = nil

      gotos = @gotos
      if @d_la
        puts "\n--- goto ---"
        gotos.each_with_index {|g,i| print i, ' '; p g }
      end

      ### initialize_LA()
      ### set_goto_map()
      la_rules = []
      @states.each do |state|
        state.check_la la_rules
      end


      ### initialize_F()
      f     = create_tmap(gotos.size)
      reads = []
      edge  = []
      gotos.each do |goto|
        goto.to_state.goto_table.each do |t, st|
          if t.terminal?
            f[goto.ident] |= (1 << t.ident)
          elsif t.nullable?
            edge.push goto.to_state.gotos[t].ident
          end
        end
        if edge.empty?
          reads.push nil
        else
          reads.push edge
          edge = []
        end
      end
      digraph f, reads
      if @d_la
        puts "\n--- F1 (reads) ---"
        print_tab gotos, reads, f
      end


      ### build_relations()
      ### compute_FOLLOWS
      path = nil
      edge = []
      lookback = Array.new(la_rules.size, nil)
      includes = []
      gotos.each do |goto|
        goto.symbol.heads.each do |ptr|
          path = record_path(goto.from_state, ptr.rule)
          g = path[-1]
          st = g ? g.to_state : goto.from_state
          if st.conflict?
            addrel lookback, st.rruleid(ptr.rule), goto
          end
          path.reverse_each do |g|
            break if     g.symbol.terminal?
            edge.push    g.ident
            break unless g.symbol.nullable?
          end
        end
        if edge.empty?
          includes.push nil
        else
          includes.push edge
          edge = []
        end
      end
      includes = transpose(includes)
      digraph f, includes
      if @d_la
        puts "\n--- F2 (includes) ---"
        print_tab gotos, includes, f
      end


      ### compute_lookaheads
      la = create_tmap(la_rules.size)
      lookback.each_with_index do |arr, i|
        if arr
          arr.each do |g|
            la[i] |= f[g.ident]
          end
        end
      end
      if @d_la
        puts "\n--- LA (lookback) ---"
        print_tab la_rules, lookback, la
      end

      la
    end
    
    def create_tmap( size )
      Array.new(size, 0)   # use Integer as bitmap
    end

    def addrel( tbl, i, item )
      if a = tbl[i]
        a.push item
      else
        tbl[i] = [item]
      end
    end

    def record_path( begst, rule )
      st = begst
      path = []
      rule.symbols.each do |t|
        goto = st.gotos[t]
        path.push goto
        st = goto.to_state
      end
      path
    end

    def transpose( rel )
      new = Array.new(rel.size, nil)
      rel.each_with_index do |arr, idx|
        if arr
          arr.each do |i|
            addrel new, i, idx
          end
        end
      end
      new
    end

    def digraph( map, relation )
      n = relation.size
      index    = Array.new(n, nil)
      vertices = []
      @infinity = n + 2

      i = nil
      index.each_index do |i|
        if not index[i] and relation[i]
          traverse i, index, vertices, map, relation
        end
      end
    end

    def traverse( i, index, vertices, map, relation )
      vertices.push i
      index[i] = height = vertices.size

      proci = nil
      if rp = relation[i]
        rp.each do |proci|
          unless index[proci]
            traverse proci, index, vertices, map, relation
          end
          if index[i] > index[proci]
            # circulative recursion !!!
            index[i] = index[proci]
          end
          map[i] |= map[proci]
        end
      end

      if index[i] == height
        while true
          proci = vertices.pop
          index[proci] = @infinity
          break if i == proci

          map[proci] |= map[i]
        end
      end
    end

    ###

    # debug method
    def print_atab( idx, tab )
      tab.each_with_index do |i,ii|
        printf '%-20s', idx[ii].inspect
        p i
      end
    end

    def print_tab( idx, rel, tab )
      tab.each_with_index do |bin,i|
        print i, ' ', idx[i].inspect, ' << '; p rel[i]
        print '  '
        each_t(@symboltable, bin) {|t| print ' ', t }
        puts
      end
    end

    # debug method
    def print_tab_i( idx, rel, tab, i )
      bin = tab[i]
      print i, ' ', idx[i].inspect, ' << '; p rel[i]
      print '  '
      each_t(@symboltable, bin) {|t| print ' ', t }
    end

    # debug method
    def printb( i )
      each_t(@symboltable, i) do |t|
        print t, ' '
      end
      puts
    end

    def each_t( tbl, set )
      i = ii = idx = nil
      0.upto( set.size ) do |i|
        (0..7).each do |ii|
          if set[idx = i * 8 + ii] == 1
            yield tbl[idx]
          end
        end
      end
    end

    #
    # resolve
    #

    def resolve( state )
      if state.conflict?
        resolve_rr state, state.ritems
        resolve_sr state, state.stokens
      else
        if state.rrules.empty?
          # shift
          state.stokens.each do |t|
            state.action[t] = @actions.shift(state.goto_table[t])
          end
        else
          # reduce
          state.defact = @actions.reduce(state.rrules[0])
        end
      end
    end

    def resolve_rr( state, r )
      pt = t = act = item = nil

      r.each do |item|
        item.each_la(@symboltable) do |t|
          act = state.action[t]
          if act
            Reduce === act or raise "Racc FATAL: #{act.class} in action table"
            #
            # can't resolve R/R conflict (on t).
            #   reduce with upper rule as default
            #
            state.rr_conflict act.rule, item.rule, t
          else
            # not conflict
            state.action[t] = @actions.reduce(item.rule)
          end
        end
      end
    end

    def resolve_sr( state, s )
      stok = rtok = goto = act = nil

      s.each do |stok|
        goto = state.goto_table[stok]
        act = state.action[stok]

        unless act
          # no conflict
          state.action[stok] = @actions.shift(goto)
        else
          unless Reduce === act
            puts 'DEBUG -------------------------------'
            p stok
            p act
            state.action.each do |k,v|
              print k.inspect, ' ', v.inspect, "\n"
            end
            raise "Racc FATAL: #{act.class} in action table"
          end

          # conflict on stok

          rtok = act.rule.prec
          case do_resolve_sr(stok, rtok)
          when :Reduce
            # action is already set

          when :Shift
            # overwrite
            act.decref
            state.action[stok] = @actions.shift(goto)

          when :Error
            act.decref
            state.action[stok] = @actions.error

          when :CantResolve
            # shift as default
            act.decref
            state.action[stok] = @actions.shift(goto)
            state.sr_conflict stok, act.rule
          end
        end
      end
    end
    
    ASSOC = {
      :Left     => :Reduce,
      :Right    => :Shift,
      :Nonassoc => :Error
    }
   
    def do_resolve_sr( stok, rtok )
      puts "resolve_sr: s/r conflict: rtok=#{rtok}, stok=#{stok}" if @d_prec

      unless rtok and rtok.prec
        puts "resolve_sr: no prec for #{rtok}(R)" if @d_prec
        return :CantResolve
      end
      rprec = rtok.prec

      unless stok and stok.prec
        puts "resolve_sr: no prec for #{stok}(S)" if @d_prec
        return :CantResolve
      end
      sprec = stok.prec

      ret = if rprec == sprec
              ASSOC[rtok.assoc] or
                  raise "Racc FATAL: #{rtok}.assoc is not Left/Right/Nonassoc"
            else
              (rprec > sprec) ? (:Reduce) : (:Shift)
            end

      puts "resolve_sr: resolved as #{ret.id2name}" if @d_prec
      ret
    end

    #
    # complete
    #

    def set_accept
      anch = @symboltable.anchor
      init_state = @states[0].goto_table[@ruletable.start]
      targ_state = init_state.action[anch].goto_state
      acc_state  = targ_state.action[anch].goto_state

      acc_state.action.clear
      acc_state.goto_table.clear
      acc_state.defact = @actions.accept
    end

    def pack( state )
      ### find most frequently used reduce rule
      act = state.action
      arr = Array.new(@ruletable.size, 0)
      t = a = nil
      act.each do |t,a|
        if Reduce === a
          arr[a.ruleid] += 1
        end
      end
      i = arr.max
      s = i>0 ? arr.index(i) : nil

      ### set & delete default action
      if s
        r = @actions.reduce(s)
        if not state.defact or state.defact == r
          act.delete_if {|t,a| a == r }
          state.defact = r
        end
      else
        state.defact ||= @actions.error
      end
    end

    def check_useless
      act = nil
      used = []
      @actions.each_reduce do |act|
        if not act or act.refn == 0
          act.rule.useless = true
        else
          t = act.rule.target
          used[t.ident] = t
        end
      end
      @symboltable.nt_base.upto(@symboltable.nt_max - 1) do |n|
        unless used[n]
          @symboltable[n].useless = true
        end
      end
    end

  end   # class StateTable


  #
  # State
  #
  # represents a LALR state.
  #

  class State

    def initialize( ident, core )
      @ident = ident
      @core  = core

      @goto_table = {}
      @gotos      = {}

      @stokens = nil
      @ritems = nil

      @action = {}
      @defact = nil

      @rrconf = nil
      @srconf = nil

      ###

      @closure = make_closure(@core)
    end

    attr_reader :ident
    alias stateid ident
    alias hash ident

    attr_reader :core
    attr_reader :closure

    attr_reader :goto_table
    attr_reader :gotos

    attr_reader :stokens
    attr_reader :ritems
    attr_reader :rrules

    attr_reader :action
    attr_accessor :defact   # default action

    attr_reader :rrconf
    attr_reader :srconf

    def inspect
      "<state #{@ident}>"
    end

    alias to_s inspect

    def ==( oth )
      @ident == oth.ident
    end

    alias eql? ==

    def make_closure( core )
      set = ISet.new
      core.each do |ptr|
        set.add ptr
        if t = ptr.dereference and t.nonterminal?
          set.update_a t.expand
        end
      end
      set.to_a
    end

    def check_la( la_rules )
      @conflict = false
      s = []
      r = []
      @closure.each do |ptr|
        if t = ptr.dereference
          if t.terminal?
            s[t.ident] = t
            if t.ident == 1    # $error
              @conflict = true
            end
          end
        else
          r.push ptr.rule
        end
      end
      unless r.empty?
        if not s.empty? or r.size > 1
          @conflict = true
        end
      end
      s.compact!
      @stokens  = s
      @rrules = r

      if @conflict
        @la_rules_i = la_rules.size
        @la_rules = r.map {|i| i.ident }
        la_rules.concat r
      else
        @la_rules_i = @la_rules = nil
      end
    end

    def conflict?
      @conflict
    end

    def rruleid( rule )
      if i = @la_rules.index(rule.ident)
        @la_rules_i + i
      else
        puts '/// rruleid'
        p self
        p rule
        p @rrules
        p @la_rules_i
        raise 'Racc FATAL: cannot get reduce rule id'
      end
    end

    def set_la( la )
      return unless @conflict

      i = @la_rules_i
      @ritems = r = []
      @rrules.each do |rule|
        r.push Item.new(rule, la[i])
        i += 1
      end
    end

    def rr_conflict( high, low, ctok )
      c = RRconflict.new(@ident, high, low, ctok)

      @rrconf ||= {}
      if a = @rrconf[ctok]
        a.push c
      else
        @rrconf[ctok] = [c]
      end
    end

    def sr_conflict( shift, reduce )
      c = SRconflict.new(@ident, shift, reduce)

      @srconf ||= {}
      if a = @srconf[shift]
        a.push c
      else
        @srconf[shift] = [c]
      end
    end

  end   # State


  #
  # Goto
  #
  # represents a transition on the grammar.
  # REAL GOTO means transition by nonterminal,
  # but this class treats also terminal's.
  # If one is a terminal transition, .ident returns nil.
  #

  class Goto

    def initialize( ident, sym, from, to )
      @ident      = ident
      @symbol     = sym
      @from_state = from
      @to_state   = to
    end

    attr_reader :ident
    attr_reader :symbol
    attr_reader :from_state
    attr_reader :to_state
    
    def inspect
      "(#{@from_state.ident}-#{@symbol}->#{@to_state.ident})"
    end
  
  end


  #
  # Item
  #
  # a LALR item. A set of rule and its lookahead tokens.
  #

  class Item
  
    def initialize( rule, la )
      @rule = rule
      @la  = la
    end

    attr_reader :rule
    attr_reader :la

    def each_la( tbl )
      la = @la
      i = ii = idx = nil
      0.upto(la.size - 1) do |i|
        (0..7).each do |ii|
          if la[idx = i * 8 + ii] == 1
            yield tbl[idx]
          end
        end
      end
    end

  end


  #
  # ActionTable
  #
  # The table of LALR actions. Actions are either of
  # Shift, Reduce, Accept and Error.
  #

  class ActionTable

    def initialize( rt, st )
      @ruletable = rt
      @statetable = st

      @reduce = []
      @shift = []
      @accept = nil
      @error = nil
    end

    def init
      @ruletable.each do |rule|
        @reduce.push Reduce.new(rule)
      end
      @statetable.each do |state|
        @shift.push Shift.new(state)
      end
      @accept = Accept.new
      @error = Error.new
    end

    def reduce_n
      @reduce.size
    end

    def reduce( i )
      case i
      when Rule    then i = i.ident
      when Integer then ;
      else
        raise "Racc FATAL: wrong class #{i.class} for reduce"
      end

      r = @reduce[i] or raise "Racc FATAL: reduce action #{i.inspect} not exist"
      r.incref
      r
    end

    def each_reduce( &block )
      @reduce.each(&block)
    end

    def shift_n
      @shift.size
    end

    def shift( i )
      case i
      when State   then i = i.ident
      when Integer then ;
      else
        raise "Racc FATAL: wrong class #{i.class} for shift"
      end

      @shift[i] or raise "Racc FATAL: shift action #{i} not exist"
    end

    def each_shift( &block )
      @shift.each(&block)
    end

    attr_reader :accept
    attr_reader :error

  end


  class Shift

    def initialize( goto )
      @goto_state = goto
    end

    attr_reader :goto_state

    def goto_id
      @goto_state.ident
    end

    def inspect
      "<shift #{@goto_state.ident}>"
    end

  end


  class Reduce

    def initialize( rule )
      @rule = rule
      @refn = 0
    end

    attr_reader :rule
    attr_reader :refn

    def ruleid
      @rule.ident
    end

    def inspect
      "<reduce #{@rule.ident}>"
    end

    def incref
      @refn += 1
    end

    def decref
      @refn -= 1
      bug! 'act.refn < 0' if @refn < 0
    end

  end


  class Accept

    def inspect
      "<accept>"
    end

  end


  class Error

    def inspect
      "<error>"
    end
  
  end


  #
  # Conflicts
  #

  class SRconflict

    def initialize( sid, shift, reduce )
      @stateid = sid
      @shift   = shift
      @reduce  = reduce
    end
    
    attr_reader :stateid
    attr_reader :shift
    attr_reader :reduce
  
    def to_s
      sprintf('state %d: S/R conflict rule %d reduce and shift %s',
              @stateid, @reduce.ruleid, @shift.to_s)
    end

  end

  class RRconflict
    
    def initialize( sid, high, low, tok )
      @stateid   = sid
      @high_prec = high
      @low_prec  = low
      @token     = tok
    end

    attr_reader :stateid
    attr_reader :high_prec
    attr_reader :low_prec
    attr_reader :token
  
    def to_s
      sprintf('state %d: R/R conflict with rule %d and %d on %s',
              @stateid, @high_prec.ident, @low_prec.ident, @token.to_s)
    end

  end

end
