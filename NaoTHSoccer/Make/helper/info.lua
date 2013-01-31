function get_info(cmd, default)
  local f = io.popen (cmd, "r");
  local rev = f:read("*a")
  f:close ();
  if rev ~= nil then 
	return rev
  end  
  return default
end

REVISION = get_info("bzr revno --tree", -1)
USER_NAME = get_info("bzr whoami", "-")
BRANCH_PATH = get_info("bzr info", "-")

REVISION = string.gsub(REVISION,"\n", "")
USER_NAME = string.gsub(USER_NAME,"\n", "")
BRANCH_PATH = string.match(BRANCH_PATH,"checkout root: (.-)\n")

print("INFO: repository info")
print("  REVISION = " .. REVISION)
print("  USER_NAME = " .. USER_NAME)
print("  BRANCH_PATH = " .. BRANCH_PATH)
print()