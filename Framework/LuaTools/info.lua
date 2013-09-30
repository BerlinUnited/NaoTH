function get_info(cmd, default)
  local f = io.popen (cmd, "r");
  local rev = f:read("*a")
  f:close ();
  if rev ~= nil then 
	return rev
  end  
  return default
end

REVISION = get_info("git rev-parse HEAD", -1)
USER_NAME = get_info("git config user.name", "-")
BRANCH_PATH = get_info("git branch -a", -1)

REVISION = string.gsub(REVISION,"\n", "")
USER_NAME = string.gsub(USER_NAME,"\n", "")

print("INFO: repository info")
print("  REVISION = " .. REVISION)
print("  USER_NAME = " .. USER_NAME)
print("  BRANCH_PATH = " .. BRANCH_PATH)
print()