which python
which python3


python3 setup.py sdist bdist_wheel
pip3 install twine
cat <<EOF >> .pypirc
[distutils]
index-servers =
    gitlab

[gitlab]
repository = https://gitlab.com/api/v4/projects/<project_id>/packages/pypi
username = gitlab-ci-token
password = $CI_JOB_TOKEN
EOF

echo "publish naoth package to Gitlab pypi"
python3 -m twine upload --repository gitlab dist/* --config-file .pypirc