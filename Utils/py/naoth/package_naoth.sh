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