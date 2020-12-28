# get version of package that is build
current_version=$(cat VERSION)
package_id=$(curl --header "PRIVATE-TOKEN: s8YpzmfyyH_FVK9o4FVq" "https://scm.cms.hu-berlin.de/api/v4/projects/3384/packages?package_type=pypi&package_name=naoth" | jq -r ".[] | select(.version == \"$current_version\").id")

#
if [ -n $package_id ]; then
  echo "try to delete existing pypi package in gitlab"
  delete_status=$(curl --request DELETE --header "PRIVATE-TOKEN: s8YpzmfyyH_FVK9o4FVq" "https://scm.cms.hu-berlin.de/api/v4/projects/3384/packages/$package_id")
  echo $delete_status
fi

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