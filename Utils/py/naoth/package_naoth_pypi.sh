echo "build naoth package"
python3 setup.py sdist bdist_wheel


cat <<EOF > .pypirc
[pypi]

username = __token__
password = $PYPI_TOKEN
EOF

echo "publish naoth package to pypi"
python3 -m twine upload dist/* --config-file .pypirc
