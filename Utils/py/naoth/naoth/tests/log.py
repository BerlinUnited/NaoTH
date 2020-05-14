import unittest

from naoth.datasets import edgels
from naoth.log._experimental_reader import Reader


class TestLogReader(unittest.TestCase):
    # TODO: Select a more simple log file
    # Note: This test requires that the log file is complete and frames are not broken
    log_path = edgels.load_data()

    def test_multiple_iterators(self):
        """
        Test for nested log file reads. Checks if two nested iterators return the same number of frames.
        """
        target_n_frames = 21623

        with Reader(self.log_path) as reader:
            n_frames_a = n_frames_b = 0

            for i, a in enumerate(reader.read()):
                n_frames_a += 1
                # After reading a bit start the other iterator
                # TODO: i depends on number of frames in log, make sure that log has enough frames
                if i == 5:
                    for b in reader.read():
                        n_frames_b += 1

            # read a and read b should generate the same number of frames
            self.assertEqual(target_n_frames, len(reader.frames))
            self.assertEqual(target_n_frames, n_frames_a)
            self.assertEqual(target_n_frames, n_frames_b)

    def test_read_size(self):
        """
        Test if the whole log file was read correctly.
        """
        parsed_size = 0
        with Reader(self.log_path) as reader:
            for frame in reader.read():
                self.assertIsNotNone(frame)
                parsed_size += len(bytes(frame))

        self.assertEqual(reader.log_file_size, parsed_size)

    def test_diet_read_size(self):
        """
        Test if the whole log file was read correctly.
        """
        parsed_size = 0
        with Reader(self.log_path) as reader:
            for frame in reader.diet_read():
                self.assertIsNotNone(frame)
                parsed_size += len(bytes(frame))

        self.assertEqual(reader.log_file_size, parsed_size)

    def test_get_item(self):
        """
        Test if the __getitem__ function returns all frames correctly.
        """
        parsed_size = 0
        with Reader(self.log_path) as reader:
            # iterating directly over the reader calls the __getitem__ function
            for frame in reader:
                self.assertIsNotNone(frame)
                parsed_size += len(bytes(frame))

        self.assertEqual(reader.log_file_size, parsed_size)
