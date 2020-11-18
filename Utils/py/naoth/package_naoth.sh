# TODO get version of package that is build
# TODO get list of released versions

# Option 1
	# TODO if version exists delete this version
	# TODO then upload
# Option 2:
	# dont upload

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