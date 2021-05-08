import os
import unittest

from naoth.log import Reader, IncompleteFrameException


def _skip_unless_env(*variables):
    """
    Skip test if any of the given environment variables are not defined
    @param vars: Any number of environment variables
    """
    for var in variables:
        try:
            value = os.environ[var]
        except KeyError:
            return unittest.skip(f'Environment variable {var} is not defined.')
    return lambda func: func


class TestLogReader(unittest.TestCase):
    @_skip_unless_env('TEST_LOG_FILE')
    def test_multiple_iterators(self):
        """
        Test of nested log file reads. Checks if two nested iterators return the same number of frames.
        """
        with Reader(os.environ['TEST_LOG_FILE']) as reader:
            n_frames_a = n_frames_b = 0

            for i, a in enumerate(reader.read()):
                n_frames_a += 1
                # After reading a bit, start the other iterator
                if i == 1:
                    for b in reader.read():
                        n_frames_b += 1

            if n_frames_a <= 2:
                raise unittest.SkipTest('Test log has not enough frames to complete the test.')

            # read a and read b should generate the same number of frames
            self.assertEqual(len(reader.frames), n_frames_a)
            self.assertEqual(len(reader.frames), n_frames_b)

    @_skip_unless_env('TEST_LOG_FILE')
    def test_read_size(self):
        """
        Test if the whole log file was read correctly.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(os.environ['TEST_LOG_FILE'], skip_incomplete=False) as reader:
            try:
                for frame in reader.read():
                    self.assertIsNotNone(frame)

                    parsed_size += len(bytes(frame))
            except IncompleteFrameException as exc:
                raise unittest.SkipTest(f'Could not complete Test: {exc}')

            self.assertEqual(reader.log_file_size, parsed_size)

    @_skip_unless_env('TEST_LOG_FILE')
    def test_diet_read_size(self):
        """
        Test if the whole log file was read correctly using the Reader.diet_read function.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(os.environ['TEST_LOG_FILE'], skip_incomplete=False) as reader:
            try:
                for frame in reader.diet_read():
                    self.assertIsNotNone(frame)
                    parsed_size += len(bytes(frame))
            except IncompleteFrameException as exc:
                raise unittest.SkipTest(f'Could not complete Test: {exc}')

            self.assertEqual(reader.log_file_size, parsed_size)

    @_skip_unless_env('TEST_LOG_FILE')
    def test_get_item(self):
        """
        Test if the __getitem__ function returns all frames correctly.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(os.environ['TEST_LOG_FILE'], skip_incomplete=False) as reader:
            try:
                # iterating directly over the reader calls the __getitem__ function
                for frame in reader:
                    self.assertIsNotNone(frame)
                    parsed_size += len(bytes(frame))
            except IncompleteFrameException as exc:
                raise unittest.SkipTest(f'Could not complete Test: {exc}')

            self.assertEqual(reader.log_file_size, parsed_size)
