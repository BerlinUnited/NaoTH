# NaoTH Python Package
*Optional: Switch to your conda or virtual env first*

run as root in the /Utils/py folder:
```bash
python -m pip install -e naoth
```
This will install the naoth project in editable mode.

or alternatively you can install the package in your home folder:

```bash
python -m pip install --user -e naoth
```
## Running the tests

- Execute run_tests.py:
```bash
python naoth/run_tests.py
```

If you want to test the log module, you need to specify a test log file,  otherwhise those tests will be skipped.
```bash
python naoth/run_tests.py --log test.log
```
