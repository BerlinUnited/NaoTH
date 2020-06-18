import unittest

from naoth.datasets import edgels
from naoth.log import Reader


# TODO: Select a smaller log file
# Note: Test requires that the log file is complete and frames are not broken.
TEST_LOG = edgels.load_data()
TEST_LOG_FRAME_COUNT = 21623


def _frame_count_more_than(log_path, number):
    """
    Checks of a given log has more than a given number of frames
    @param log_path: path to log file to check
    @param number: number of frames
    @returns True if condition was met
    """
    n_frames = 0
    with Reader(log_path) as reader:
        for frame in reader.read():
            n_frames += 1
            if n_frames > number:
                return True
    return False


class TestLogReader(unittest.TestCase):
    @unittest.skipIf(not _frame_count_more_than(TEST_LOG, 2), 'Test log has not enough frames to complete the test.')
    def test_multiple_iterators(self):
        """
        Test of nested log file reads. Checks if two nested iterators return the same number of frames.
        """
        with Reader(TEST_LOG) as reader:
            n_frames_a = n_frames_b = 0

            for i, a in enumerate(reader.read()):
                n_frames_a += 1
                # After reading a bit, start the other iterator
                if i == 1:
                    for b in reader.read():
                        n_frames_b += 1

            # read a and read b should generate the same number of frames
            self.assertEqual(TEST_LOG_FRAME_COUNT, len(reader.frames))
            self.assertEqual(TEST_LOG_FRAME_COUNT, n_frames_a)
            self.assertEqual(TEST_LOG_FRAME_COUNT, n_frames_b)

    def test_read_size(self):
        """
        Test if the whole log file was read correctly.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(TEST_LOG) as reader:
            for frame in reader.read():
                self.assertIsNotNone(frame)

                parsed_size += len(bytes(frame))

            self.assertEqual(reader.log_file_size, parsed_size)

    def test_diet_read_size(self):
        """
        Test if the whole log file was read correctly.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(TEST_LOG) as reader:
            for frame in reader.diet_read():
                self.assertIsNotNone(frame)
                parsed_size += len(bytes(frame))

            self.assertEqual(reader.log_file_size, parsed_size)

    def test_get_item(self):
        """
        Test if the __getitem__ function returns all frames correctly.
        Note: Requires a complete test log file without broken frames.
        """
        parsed_size = 0
        with Reader(TEST_LOG) as reader:
            # iterating directly over the reader calls the __getitem__ function
            for frame in reader:
                self.assertIsNotNone(frame)
                parsed_size += len(bytes(frame))

            self.assertEqual(reader.log_file_size, parsed_size)
