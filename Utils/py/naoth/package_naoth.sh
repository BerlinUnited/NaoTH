# get version of package that is build
current_version=$(cat VERSION)

# get list of released versions
version_array=($(curl --header "PRIVATE-TOKEN: s8YpzmfyyH_FVK9o4FVq" "https://scm.cms.hu-berlin.de/api/v4/projects/3384/packages?package_type=pypi&package_name=naoth" | jq '.[] | .version'))

for version in "${version_array[@]}"
do
   :
   # strip quotes
  version=$(echo "$version" | tr -d '"')
  if [[ $version == $current_version ]]; then
    package_exists=true
  fi
done


echo "package_exists" $package_exists

if [ -z $package_exists ]; then
echo "build naoth package"
  python3 setup.py sdist bdist_wheel

  # $PYPI_TOKEN comes from gitlab settings

cat <<EOF >> .pypirc
[distutils]
index-servers =
    gitlab

[gitlab]
repository = https://scm.cms.hu-berlin.de/api/v4/projects/3384/packages/pypi
username = __token__
password = $PYPI_TOKEN
EOF

echo "publish naoth package to Gitlab pypi"
python3 -m twine upload --repository gitlab dist/* --config-file .pypirc
else
  echo "package already exists"
fi


# Option 1
	# TODO if version exists delete this version
	# TODO then upload
# Option 2:
	# dont upload

