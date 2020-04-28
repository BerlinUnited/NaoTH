python setup.py sdist bdist_wheel
pip install twine
cat <<EOF >> .pypirc
[distutils]
index-servers =
    gitlab

[gitlab]
repository = https://gitlab.com/api/v4/projects/<project_id>/packages/pypi
username = __token__
password = <your personal access token>
EOF

echo "publish naoth package to Gitlab pypi"
python3 -m twine upload --repository gitlab dist/* --config-file .pypirc