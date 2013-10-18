function get_info(cmd, default)
  local f = io.popen (cmd, "r");
  local rev = nil
  if f ~= nil then
  	rev = f:read("*a")
		f:close ();
  end
  
  if rev == nil then 
		rev = default
  end  
	return rev
end

REVISION = get_info("git rev-parse HEAD", -1)
USER_NAME = get_info("git config user.name", -1)
BRANCH_PATH = get_info("git branch -a", "  no branch info\n")

REVISION = string.gsub(REVISION,"\n", "")
USER_NAME = string.gsub(USER_NAME,"\n", "")
BRANCH_PATH = "(* indicates active branch in following list)\n  [\n  " .. string.gsub(BRANCH_PATH,"\n", "\n  ") .."]"

print("INFO: repository info")
print("  REVISION = " .. REVISION)
print("  USER_NAME = " .. USER_NAME)
print("  BRANCH_PATH = " .. BRANCH_PATH)
print()

BRANCH_PATH = "todo"