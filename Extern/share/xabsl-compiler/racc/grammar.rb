#
# grammar.rb
#
# Copyright (c) 1999-2003 Minero Aoki <aamine@loveruby.net>
#
# This program is free software.
# You can distribute/modify this program under the terms of
# the GNU LGPL, Lesser General Public License version 2.
# For details of the GNU LGPL, see the file "COPYING".
#

require 'racc/compat'
require 'racc/iset'


module Racc

  class UserAction
  
    def initialize( str, lineno )
      @val = (str.strip.empty? ? nil : str)
      @lineno = lineno
    end

    attr_reader :val
    attr_reader :lineno

    def name
      '{action}'
    end

    alias inspect name
  
  end


  class OrMark

    def initialize( lineno )
      @lineno = lineno
    end

    def name
      '|'
    end

    alias inspect name

    attr_reader :lineno

  end


  class Prec
  
    def initialize( tok, lineno )
      @val = tok
      @lineno = lineno
    end

    def name
      '='
    end

    alias inspect name

    attr_reader :val
    attr_reader :lineno
  
  end


  #########################################################################
  ###########################              ################################
  ###########################     rule     ################################
  ###########################              ################################
  #########################################################################


  #
  # RuleTable
  #
  # stands grammar. Each items of @rules are Rule object.
  #

  class RuleTable

    def initialize( racc )
      @racc        = racc
      @symboltable = racc.symboltable

      @d_token = racc.d_token
      @d_rule  = racc.d_rule
      @d_state = racc.d_state

      @rules   = []
      @hashval = 4   # size of dummy rule
      @start   = nil
      @sprec   = nil
      @emb     = 1
      @expect  = nil

      @end_rule = false
    end

    ###
    ### register
    ###

    def register_rule_from_array( arr )
      sym = arr.shift
      case sym
      when OrMark, UserAction, Prec
        raise ParseError, "#{sym.lineno}: unexpected token #{sym.name}"
      end
      new = []
      arr.each do |i|
        case i
        when OrMark
          register_rule sym, new
          new = []
        when Prec
          raise ParseError, "'=<prec>' used twice in one rule" if @sprec
          @sprec = i.val
        else
          new.push i
        end
      end
      register_rule sym, new
    end
    
    def register_rule( targ, list )
      if targ
        @prev_target = targ
      else
        targ = @prev_target
      end

      if UserAction === list[-1]
        act = list.pop
      else
        act = UserAction.new('', 0)
      end
      list.map! {|t| (UserAction === t) ? embed_symbol(t) : t }

      reg_rule targ, list, act
      @start ||= targ
      @sprec = nil
    end

    def reg_rule( targ, list, act )
      @rules.push Rule.new(targ, list, act, @rules.size + 1, @hashval, @sprec)
      @hashval += (list.size + 1)
    end

    def embed_symbol( act )
      sym = @symboltable.get("@#{@emb}".intern, true)
      @emb += 1
      reg_rule sym, [], act
      sym
    end

    def end_register_rule
      @end_rule = true
      raise RaccError, 'no rule in input' if @rules.empty?
    end

    def register_start( tok )
      raise ParseError, "'start' defined twice'" if @start
      @start = tok
    end

    def register_option( option )
      case option.sub(/\Ano_/, '')
      when 'omit_action_call'
        @racc.omit_action = ((/\Ano_/ === option) ? false : true)
      when 'result_var'
        @racc.result_var = ((/\Ano_/ === option) ? false : true)
      else
        raise ParseError, "unknown option '#{option}'"
      end
    end

    def expect( n = nil )
      return @expect unless n
      raise ParseError, "'expect' exist twice" if @expect
      @expect = n
    end

    ###
    ### accessor
    ###

    attr_reader :start

    def []( x )
      @rules[x]
    end

    def each_rule( &block )
      @rules.each(&block)
    end

    alias each each_rule

    def each_index( &block )
      @rules.each_index(&block)
    end

    def each_with_index( &block )
      @rules.each_with_index(&block)
    end

    def size
      @rules.size
    end

    def to_s
      "<Racc::RuleTable>"
    end

    ###
    ### process
    ###

    def init
      #
      # add dummy rule
      #
      tmp = Rule.new(@symboltable.dummy,
                     [ @start, @symboltable.anchor, @symboltable.anchor ],
                     UserAction.new('', 0),
                     0, 0, nil)
                    # id hash prec
      @rules.unshift tmp
      @rules.freeze

      rule = ptr = tmp = tok = t = nil

      #
      # t.heads
      #
      @rules.each do |rule|
        rule.target.heads.push rule.ptrs[0]
      end

      #
      # t.terminal?, self_null?
      #
      @symboltable.each do |t|
        t.term = t.heads.empty?
        if t.terminal?
          t.snull = false
          next
        end

        tmp = false
        t.heads.each do |ptr|
          if ptr.reduce?
            tmp = true
            break
          end
        end
        t.snull = tmp
      end

      @symboltable.fix

      #
      # t.locate
      #
      @rules.each do |rule|
        tmp = nil
        rule.ptrs.each do |ptr|
          unless ptr.reduce?
            tok = ptr.dereference
            tok.locate.push ptr
            tmp = tok if tok.terminal?
          end
        end
        rule.set_prec tmp
      end

      #
      # t.expand
      #
      @symboltable.each_nonterm {|t| compute_expand t }

      #
      # t.nullable?, rule.nullable?
      #
      compute_nullable

      #
      # t.useless?, rule.useless?
      #
      compute_useless
    end

    def compute_expand( t )
      puts "expand> #{t.to_s}" if @d_token
      t.expand = _compute_expand(t, ISet.new, [])
      puts "expand< #{t.to_s}: #{t.expand.to_s}" if @d_token
    end

    def _compute_expand( t, ret, lock )
      if tmp = t.expand
        ret.update tmp
        return ret
      end

      tok = h = nil

      ret.update_a t.heads
      t.heads.each do |ptr|
        tok = ptr.dereference
        if tok and tok.nonterminal?
          unless lock[tok.ident]
            lock[tok.ident] = true
            _compute_expand tok, ret, lock
          end
        end
      end

      ret
    end

    def compute_nullable
      @rules.each       {|r| r.null = false }
      @symboltable.each {|t| t.null = false }

      r = @rules.dup
      s = @symboltable.nonterminals

      begin
        rs = r.size
        ss = s.size
        check_r_nullable r
        check_s_nullable s
      end until rs == r.size and ss == s.size
    end

    def check_r_nullable( r )
      r.delete_if do |rl|
        rl.null = true
        rl.symbols.each do |t|
          unless t.nullable?
            rl.null = false
            break
          end
        end

        rl.nullable?
      end
    end

    def check_s_nullable( s )
      s.delete_if do |t|
        t.heads.each do |ptr|
          if ptr.rule.nullable?
            t.null = true
            break
          end
        end
        t.nullable?
      end
    end

    ###
    ### WHAT IS "USELESS"?
    ###
    def compute_useless
      t = del = save = nil

      @symboltable.each_terminal {|t| t.useless = false }
      @symboltable.each_nonterm  {|t| t.useless = true }
      @rules.each {|r| r.useless = true }

      r = @rules.dup
      s = @symboltable.nonterminals
      begin
        rs = r.size
        ss = s.size
        check_r_useless r
        check_s_useless s
      end until r.size == rs and s.size == ss
    end
    
    def check_r_useless( r )
      t = rule = nil
      r.delete_if do |rule|
        rule.useless = false
        rule.symbols.each do |t|
          if t.useless?
            rule.useless = true
            break
          end
        end
        not rule.useless?
      end
    end

    def check_s_useless( s )
      t = ptr = nil
      s.delete_if do |t|
        t.heads.each do |ptr|
          unless ptr.rule.useless?
            t.useless = false
            break
          end
        end
        not t.useless?
      end
    end

  end   # class RuleTable


  #
  # Rule
  #
  # stands one rule of grammar.
  #

  class Rule

    def initialize( targ, syms, act, rid, hval, prec )
      @target  = targ
      @symbols = syms
      @action  = act.val
      @lineno  = act.lineno
      @ident   = rid
      @hash    = hval
      @prec = @specified_prec = prec

      @null    = nil
      @useless = nil

      @ptrs = tmp = []
      syms.each_with_index do |t,i|
        tmp.push LocationPointer.new(self, i, t)
      end
      tmp.push LocationPointer.new(self, syms.size, nil)
    end

    attr_reader :target
    attr_reader :symbols

    attr_reader :action
    attr_reader :lineno

    attr_reader :ident
    attr_reader :hash
    attr_reader :ptrs

    attr_reader :prec
    attr_reader :specified_prec

    def set_prec( t )
      @prec ||= t
    end

    def nullable?() @null end
    def null=(n)    @null = n end

    def useless?()  @useless end
    def useless=(u) @useless = u end

    def inspect
      "#<rule #{@ident} (#{@target})>"
    end

    def ==( other )
      Rule === other and @ident == other.ident
    end

    def []( idx )
      @symbols[idx]
    end

    def size
      @symbols.size
    end

    def empty?
      @symbols.empty?
    end

    def to_s
      '#<rule#{@ident}>'
    end

    def accept?
      if tok = @symbols[-1]
        tok.anchor?
      else
        false
      end
    end

    def each( &block )
      @symbols.each(&block)
    end

  end   # class Rule


  #
  # LocationPointer
  #
  # set of rule and position in it's rhs.
  # note that number of pointer is more than rule's rhs array,
  # because pointer points right of last symbol when reducing.
  #

  class LocationPointer

    def initialize( rule, i, sym )
      @rule   = rule
      @index  = i
      @symbol = sym
      @ident  = @rule.hash + i
      @reduce = sym.nil?
    end

    attr_reader :rule
    attr_reader :index
    attr_reader :symbol

    alias dereference symbol

    attr_reader :ident
    alias hash ident
    attr_reader :reduce
    alias reduce? reduce

    def to_s
      sprintf('(%d,%d %s)',
              @rule.ident, @index, (reduce?() ? '#' : @symbol.to_s))
    end

    alias inspect to_s

    def eql?( ot )
      @hash == ot.hash
    end

    alias == eql?

    def head?
      @index == 0
    end

    def next
      @rule.ptrs[@index + 1] or ptr_bug!
    end

    alias increment next

    def before( len )
      @rule.ptrs[@index - len] or ptr_bug!
    end

    private
    
    def ptr_bug!
      bug! "pointer not exist: self: #{to_s}"
    end

  end   # class LocationPointer


  #########################################################################
  ###########################              ################################
  ###########################    symbol    ################################
  ###########################              ################################
  #########################################################################

  #
  # SymbolTable
  #
  # the table of symbols.
  # each items of @symbols are Sym
  #

  class SymbolTable

    include Enumerable

    def initialize( racc )
      @chk        = {}
      @symbols    = []
      @token_list = nil
      @prec_table = []

      @end_conv = false
      @end_prec = false
      
      @dummy  = get(:$start, true)
      @anchor = get(:$end,   true)   # ID 0
      @error  = get(:error, false)   # ID 1

      @anchor.conv = 'false'
      @error.conv = 'Object.new'
    end

    attr_reader :dummy
    attr_reader :anchor
    attr_reader :error

    def get( val, dummy = false )
      unless ret = @chk[val]
        @chk[val] = ret = Sym.new(val, dummy)
        @symbols.push ret
      end
      ret
    end


    def register_token( toks )
      @token_list ||= []
      @token_list.concat toks
    end


    def register_prec( assoc, toks )
      if @end_prec
        raise ParseError, "'prec' block is defined twice"
      end
      toks.push assoc
      @prec_table.push toks
    end

    def end_register_prec( rev )
      @end_prec = true

      return if @prec_table.empty?

      top = @prec_table.size - 1
      @prec_table.each_with_index do |toks, idx|
        ass = toks.pop
        toks.each do |tok|
          tok.assoc = ass
          if rev
            tok.prec = top - idx
          else
            tok.prec = idx
          end
        end
      end
    end


    def register_conv( tok, str )
      if @end_conv
        raise ParseError, "'convert' block is defined twice"
      end
      tok.conv = str
    end

    def end_register_conv
      @end_conv = true
    end


    def fix
      #
      # initialize table
      #
      term = []
      nt = []
      t = i = nil
      @symbols.each do |t|
        (t.terminal? ? term : nt).push t
      end
      @symbols = term + nt
      @nt_base = term.size
      @terms = terminals
      @nterms = nonterminals

      @symbols.each_with_index do |t, i|
        t.ident = i
      end

      return unless @token_list

      #
      # check if decleared symbols are really terminal
      #
      toks = @symbols[2, @nt_base - 2]
      @token_list.uniq!
      @token_list.each do |t|
        unless toks.delete t
          $stderr.puts "racc warning: terminal #{t} decleared but not used"
        end
      end
      toks.each do |t|
        unless String === t.value
          $stderr.puts "racc warning: terminal #{t} used but not decleared"
        end
      end
    end


    def []( id )
      @symbols[id]
    end

    attr_reader :nt_base

    def nt_max
      @symbols.size
    end

    def each( &block )
      @symbols.each(&block)
    end

    def terminals( &block )
      @symbols[0, @nt_base]
    end

    def each_terminal( &block )
      @terms.each(&block)
    end

    def nonterminals
      @symbols[@nt_base, @symbols.size - @nt_base]
    end

    def each_nonterm( &block )
      @nterms.each(&block)
    end

  end


  #
  # Sym
  #
  # stands symbol (terminal and nonterminal).
  # This class is not named Symbol because there is
  # a class 'Symbol' after ruby 1.5.
  #

  class Sym

    def initialize( val, dummy )
      @ident = nil
      @value = val
      @dummy = dummy

      @term  = nil
      @nterm = nil
      @conv  = nil
      @prec  = nil

      @heads    = []
      @locate   = []
      @snull    = nil
      @null     = nil
      @expand   = nil

      @useless  = nil

      # for human
      @to_s = if @value.respond_to?(:id2name)
              then @value.id2name
              else @value.to_s.inspect
              end
      # for ruby source
      @uneval = if @value.respond_to?(:id2name)
                then ':' + @value.id2name
                else @value.to_s.inspect
                end
    end


    class << self
      def once_writer( nm )
        nm = nm.id2name
        module_eval(<<-EOS)
          def #{nm}=( v )
            bug! unless @#{nm}.nil?
            @#{nm} = v
          end
        EOS
      end
    end

    #
    # attributes
    #

    once_writer :ident
    attr_reader :ident

    alias hash ident

    attr_reader :value

    def dummy?() @dummy end

    def terminal?()    @term end
    def nonterminal?() @nterm end

    def term=( t )
      bug! unless @term.nil?
      @term = t
      @nterm = !t
    end

    def conv=( str ) @conv = @uneval = str end
    attr_reader :conv

    attr_accessor :prec
    attr_accessor :assoc

    def to_s()   @to_s.dup end
    def uneval() @uneval.dup end
    alias inspect to_s

    #
    # computed
    #

    attr_reader :heads
    attr_reader :locate

    once_writer :snull
    def self_null?() @snull end

    def nullable?() @null end
    def null=(n)    @null = n end

    once_writer :expand
    attr_reader :expand

    def useless=(f) @useless = f end
    def useless?() @useless end

  end   # class Sym

end   # module Racc
