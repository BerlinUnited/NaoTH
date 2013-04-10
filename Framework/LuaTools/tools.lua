function os.copydir(from, to, pattern)

  if not from:endswith("/") then
    from = from .. "/"
  end
  if not to:endswith("/") then
    to = to .. "/"
  end

  pattern = pattern or "**"

  -- make sure target directories exist
  local dirs = os.matchdirs(from .. pattern)
  for i,d in ipairs(dirs) do
    local dRelFrom = path.getrelative(from, d)
    local newPath =  to  .. dRelFrom
    os.mkdir(newPath)
  end
  
  -- copy the files itself
  local files = os.matchfiles(from .. pattern)
  for i,f in ipairs(files) do
    local fRelFrom = path.getrelative(from, f)
    local newPath = to .. fRelFrom
    os.copyfile(f,newPath)
  end

end -- os.copydir

function path.toExecutable(path)
  if os.is("windows") then
    return path .. ".exe"
  elseif os.is("macosx") then
    return path .. ".app"
  else
    -- do nothing, real operatingsystems don't use .exe :-)
    return path
  end -- check OS
end -- path.toExecuteable
