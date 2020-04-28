apt-get update
apt-get install -y python3 python3-pip
pip3 install twine
pip3 install --upgrade keyrings.alt  # https://github.com/python-poetry/poetry/issues/1968

python3 setup.py sdist bdist_wheel

cat <<EOF >> .pypirc
[distutils]
index-servers =
    gitlab

[gitlab]
repository = https://gitlab.com/api/v4/projects/3384/packages/pypi
username = gitlab-ci-token
password = $CI_JOB_TOKEN
EOF

echo "publish naoth package to Gitlab pypi"
python3 -m twine upload --repository gitlab dist/* --config-file .pypirc