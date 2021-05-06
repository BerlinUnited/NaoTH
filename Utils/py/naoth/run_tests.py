#! /usr/bin/env python3
import argparse
import os
import sys
import unittest


def naoth_test_suite():
    suite = unittest.TestLoader().discover(start_dir='tests')
    return suite


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run naoth package tests.')
    parser.add_argument('-l', '--log', help='Test log file for testing the log parser')
    parser.add_argument('--failure-if-skipped', action='store_true', help='Exit with failure if tests were skipped')
    args = parser.parse_args()

    if args.log is not None:
        os.environ["TEST_LOG_FILE"] = args.log

    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(naoth_test_suite())

    if args.failure_if_skipped and result.skipped:
        sys.exit('Failure - Tests were skipped.')
