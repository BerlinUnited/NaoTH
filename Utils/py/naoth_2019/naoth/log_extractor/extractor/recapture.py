import asyncio
import logging
import os
import shutil
from tempfile import TemporaryDirectory
from zipfile import ZipFile

from naoth.connection import Command, Response
from naoth.log_extractor.extractor import Extractor
from naoth.logsimulator import LogSimulator

logger = logging.getLogger(__name__)


class LogRecapture(Extractor):
    """
    Recapture log files with representations calculated by the log simulator.
    """

    name = 'Recapture logs'

    def extract(self, target_log, output_path, *args, **kwargs):
        job_id = kwargs['job_id']
        target_folder = os.path.dirname(target_log)

        tmp_dir = None
        config_zip_path = None
        # looking for config folder
        for file in os.listdir(target_folder):
            if file.lower() == 'config.zip':
                config_zip_path = os.path.join(target_folder, file)
                tmp_dir = TemporaryDirectory(prefix='logsim_cwd_')
                logger.info(f'Creating temporary directory "{tmp_dir.name}" to unzip {file}...')
                with ZipFile(config_zip_path) as config_zip:
                    config_zip.extractall(tmp_dir.name)
                target_config = os.path.join(tmp_dir.name, 'Config')
                output_config = os.path.join(os.path.dirname(output_path), file)
                break
            elif file == 'Config':
                target_config = os.path.join(target_folder, file)
                output_config = os.path.join(os.path.dirname(output_path), file)
                break
        else:
            raise FileNotFoundError(f'Config for target log not found in "{target_folder}"!')

        logsim = LogSimulator(target_log, target_config, port=7000+job_id)

        # create event loop to run and communicate with log simulator process
        loop = asyncio.get_event_loop()
        loop.run_until_complete(self.recapture(logsim, output_path))

        logger.info(f'Copying Config to "{output_config}"')
        if config_zip_path is not None:
            # Copy config zip
            shutil.copy2(config_zip_path, output_config)
            tmp_dir.cleanup()
        else:
            shutil.copytree(target_config, output_config)

        logger.info(f'Done.')

    @staticmethod
    async def recapture(logsim, output_path):
        logger.info(f'Creating log simulator instance at port {logsim.port}...')
        instance = await logsim.create_process()

        await instance.boot()

        logger.info(f'Connecting with log simulator instance...')
        robot_reader, robot_writer = await asyncio.open_connection(host='127.0.0.1', port=logsim.port,
                                                                   loop=asyncio.get_event_loop())

        cmd_id = 1
        command = Command('Cognition:representation:get', cmd_id)
        cmd_id += 1
        command.add_arg('RobotInfo')
        # send command
        robot_writer.write(bytes(command))

        # disable some modules
        for module in ['ScanLineEdgelDetector', 'CameraMatrixFinder']:
            command = Command('Cognition:modules:set', cmd_id)
            cmd_id += 1
            command.add_arg(module, value='off'.encode())
            # send command
            robot_writer.write(bytes(command))

        # disable frame skip
        command = Command('Cognition:ParameterList:set', cmd_id)
        cmd_id += 1
        command.add_arg('<name>', value='DebugParameter'.encode())
        command.add_arg('log.skipTimeMS', value='0'.encode())
        # send command
        robot_writer.write(bytes(command))

        logger.info(f'Enabling recording in "{output_path}"...')
        command = Command('Cognition:CognitionLog', cmd_id)
        cmd_id += 1
        command.add_arg('open', value=output_path.encode())
        # send command
        robot_writer.write(bytes(command))

        # HACK: wait for commands to come through (hopefully)
        await asyncio.sleep(1)

        # advance log simulator frame so the responses are sent
        await instance.advance()

        # parse responses
        response = Response()
        await response.read(robot_reader)
        logger.info(f'Response: {response.data.decode().strip()}')
        await response.read(robot_reader)
        logger.info(f'Response: {response.data.decode().strip()}')
        await response.read(robot_reader)
        logger.info(f'Response: {response.data.decode().strip()}')
        await response.read(robot_reader)
        logger.info(f'Response: {response.data.decode().strip()}')
        await response.read(robot_reader)
        logger.info(f'Response: {response.data.decode().strip()}')

        # representations that can be recorded
        representations = response.data.decode().split()

        to_record = instance.logfile_representations + ['LineGraphPercept']

        # activate representations to be recorded
        for representation in to_record:
            logger.info(f'Activating recording of {representation}...')
            command = Command('Cognition:CognitionLog', cmd_id)
            cmd_id += 1
            command.add_arg('activate', value=representation.encode())
            # send command
            robot_writer.write(bytes(command))

        logger.info('Starting recorder...')
        command = Command('Cognition:CognitionLog', cmd_id)
        cmd_id += 1
        command.add_arg('on')
        # send command
        robot_writer.write(bytes(command))

        # HACK: wait for commands to come through (hopefully)
        await asyncio.sleep(1)

        await instance.advance()

        # read all representation acks
        for representation in to_record:
            await response.read(robot_reader)

        # read recording ack
        await response.read(robot_reader)
        logger.info(response.data.decode())

        logger.info('Playing log simulator...')
        await instance.play()

        logger.info(f'Shutting down log simulator instance...')
        await instance.shutdown()


if __name__ == '__main__':
    # setup logging
    import naoth.logging_config.default as log

    log.configure()

    import argparse
    parser = argparse.ArgumentParser(description=LogRecapture.__doc__)

    def is_file(path):
        if not os.path.isfile(path):
            parser.error(f'Given argument "{path}" is not a valid file path!')
            return
        return path

    parser.add_argument('target_log', type=is_file)
    parser.add_argument('output_path')
    parser.add_argument('-e', '--executable')
    args = parser.parse_args()

    os.environ['LOG_SIMULATOR_PATH'] = args.executable

    extractor = LogRecapture()
    extractor.extract(args.target_log, args.output_path, job_id=1)
