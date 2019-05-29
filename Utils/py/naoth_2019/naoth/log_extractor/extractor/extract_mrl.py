from naoth.behavior_parser import BehaviorParser
from naoth.log_extractor.extractor import Extractor
from naoth.logreader import LogReader


class ExtractGlobalLocalization(Extractor):
    """
    Extracts initial localization in the ready state from the log file.
    """
    name = 'Extract global localization'

    def extract(self, target_log, output_path, *args, **kwargs):
        behavior = BehaviorParser()

        behavior_state_complete = None

        filtered_frames = []
        with LogReader(target_log) as log_reader:
            for frame in log_reader.read():
                record = False

                if 'BehaviorStateComplete' in frame:
                    behavior.initialize(frame)

                    behavior_state_complete = frame.members['BehaviorStateComplete']

                if 'BehaviorStateSparse' in frame:
                    behavior.parse(frame)

                    if 'motion.type' in behavior.output_symbols and 'game.current_mode' in behavior.output_symbols:
                        if behavior.output_symbols['game.current_mode'] == 'ready' and \
                                behavior.output_symbols['motion.type'] == 'stand':
                            record = True

                if record:
                    filtered_frames.append(frame)
                elif filtered_frames:
                    break

            if filtered_frames:
                # add BehaviorStateComplete to first frame of the new log file
                if 'BehaviorStateComplete' not in filtered_frames[0]:
                    filtered_frames[0].add_member('BehaviorStateComplete', *behavior_state_complete)

                # write to new log file
                with open(output_path, 'wb') as output:
                    for frame in filtered_frames:
                        output.write(bytes(frame))