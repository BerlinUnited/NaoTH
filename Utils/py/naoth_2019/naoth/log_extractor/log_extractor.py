import logging
import os
import time
import traceback
from multiprocessing.pool import Pool

from PyQt5 import QtWidgets, QtCore
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QColor, QBrush
from PyQt5.QtWidgets import QTreeWidgetItem, QListWidgetItem, QFileDialog
from naoth.log_extractor.log_extractor_ui.log_extractor_form import Ui_log_file_extractor_widget
from naoth.log_extractor.log_extractor_ui.widgets import ItemContainer

logger = logging.getLogger(__name__)


class JobExecutor(QtCore.QObject):
    """
    Runs jobs in a process pool.
    """
    job_finished = QtCore.pyqtSignal(object, object)
    job_failed = QtCore.pyqtSignal(object, object)

    def __init__(self, number_of_processes=4):
        super().__init__()

        self.executors = Pool(processes=number_of_processes)
        self.jobs = {}

        self.thread = QtCore.QThread()
        self.moveToThread(self.thread)
        self.thread.started.connect(self._run)

    def add_job(self, _id, job_function, args, kwargs):
        self.jobs[_id] = (job_function, args, kwargs)

    def _run(self):
        logger.debug(f'Executing {len(self.jobs)} jobs...')

        results = {}
        for _id, (job_function, args, kwargs) in self.jobs.items():
            result = self.executors.apply_async(job_function, args, kwargs)
            results[_id] = result

        # wait for results to come in
        while results:
            for _id, result in results.items():
                if result.ready():
                    finished = _id
                    break
            else:
                time.sleep(.33)
                continue

            finished_result = results.pop(finished)

            # retrieve result
            try:
                result = finished_result.get()
                self.job_finished.emit(_id, result)
            except Exception:
                self.job_failed.emit(_id, traceback.format_exc())

        logger.debug('Jobs done.')
        self.jobs.clear()
        self.thread.terminate()

    def start(self):
        self.thread.start()


class JobTreeItem(QTreeWidgetItem):
    def __init__(self, extractor, target_path, output_path):
        args = [extractor.name, target_path, output_path, 'pending...']
        super().__init__(args)

        # set widget args as tool tip to be able to view the full text
        for i, arg in enumerate(args[:-1]):
            self.setToolTip(i, arg)

        self.extractor = extractor

        self.failed = False
        self.done = False

    def get_target(self):
        return self.text(1)

    def get_output(self):
        return self.text(2)

    def _set_progress(self, text, color='black'):
        self.setText(3, text)
        for i in range(4):
            self.setForeground(i, QBrush(QColor(color)))

    def set_pending(self):
        self.done = False
        self.failed = False
        self._set_progress('pending...')

    def set_done(self):
        self.done = True
        self._set_progress('done', color='green')

    def set_failed(self):
        self.failed = True
        self._set_progress('failed', color='red')


class LogExtractorWidget(QtWidgets.QWidget):
    job_execution_finished = pyqtSignal()

    def __init__(self, parent=None):
        super().__init__(parent=parent)

        self.ui = Ui_log_file_extractor_widget()
        self.ui.setupUi(self)

        # button events
        self.ui.job_button.clicked.connect(self.add_jobs)
        self.ui.browse_button.clicked.connect(self.browse_files)
        self.ui.execute_button.clicked.connect(self.execute_jobs)
        self.ui.clear_job_list_button.clicked.connect(self.clear_job_list)
        self.ui.clear_file_tree_button.clicked.connect(self.clear_file_tree)

        # create job executor
        self.job_executor = JobExecutor()
        self.job_executor.job_finished.connect(self.job_done)
        self.job_executor.job_failed.connect(self.job_failed)

        self.number_of_finished_jobs = 0

    @staticmethod
    def _set_label_color(label: QtWidgets.QLabel, color):
        label.setStyleSheet(f'QLabel {{color : {color};}}')

    def _enable_ui(self, boolean):
        self.ui.execute_button.setEnabled(boolean)
        self.ui.job_button.setEnabled(boolean)
        self.ui.clear_job_list_button.setEnabled(boolean)

    def _progress(self):
        self.ui.progress_bar.setMaximum(100)

        self.number_of_finished_jobs += 1
        # set progress bar
        if self.number_of_finished_jobs == self.ui.job_list.topLevelItemCount():
            self._enable_ui(True)
            self.ui.progress_bar.setValue(100)
        else:
            progress = self.number_of_finished_jobs / self.ui.job_list.topLevelItemCount()
            self.ui.progress_bar.setValue(progress * 100)

    def job_done(self, _id, results):
        job_item = self.ui.job_list.topLevelItem(_id)
        job_item.set_done()
        self._progress()

    def job_failed(self, _id, traceback):
        job_item = self.ui.job_list.topLevelItem(_id)
        job_item.set_failed()

        logger.error(f'Execution of job "{job_item.extractor.name}" failed!\n{traceback}')

        self._progress()

    def _check_missing(self):
        missing = False

        for _ in self.ui.file_tree.selected_files():
            self._set_label_color(self.ui.files_label, 'black')
            break
        else:
            missing = True
            self._set_label_color(self.ui.files_label, 'red')

        if not self.ui.extractor_list.selectedItems():
            missing = True
            self._set_label_color(self.ui.extractor_label, 'red')
        else:
            self._set_label_color(self.ui.extractor_label, 'black')

        if self.ui.output_selection.text().endswith('...') or not os.path.isdir(self.ui.output_selection.text()):
            missing = True
            self._set_label_color(self.ui.output_label, 'red')
        else:
            self._set_label_color(self.ui.output_label, 'black')
        return missing

    def _jobs_missing(self):
        if not self.ui.job_list.topLevelItemCount():
            self._set_label_color(self.ui.add_jobs_label, 'red')
            return True
        else:
            self._set_label_color(self.ui.add_jobs_label, 'black')
            return False

    def execute_jobs(self):
        if self._jobs_missing():
            return

        self.number_of_finished_jobs = 0
        self.ui.progress_bar.setValue(0)
        # use progress bar as busy indicator
        self.ui.progress_bar.setMaximum(0)

        jobs_added = False
        for i in range(self.ui.job_list.topLevelItemCount()):
            job_item = self.ui.job_list.topLevelItem(i)
            if not job_item.done:
                self._enable_ui(False)

                # create output path
                path = os.path.dirname(os.path.realpath(job_item.get_output()))
                logger.debug(f'Creating output path "{path}".')
                os.makedirs(path, exist_ok=True)

                args = (job_item.get_target(), job_item.get_output())
                kwargs = {'job_id': i}
                self.job_executor.add_job(i, job_item.extractor.extract, args, kwargs)
                jobs_added = True
            else:
                self._progress()

        # start thread
        if jobs_added:
            self.job_executor.start()

    def browse_files(self):
        directory = QFileDialog.getExistingDirectory(self)
        self.ui.output_selection.setText(directory)

    def clear_job_list(self):
        self.ui.job_list.clear()

    def clear_file_tree(self):
        self.ui.file_tree.clear()

    def add_jobs(self):
        """
        Add jobs (selected extractors x selected files) to the job list
        """
        if self._check_missing():
            return

        output_path = self.ui.output_selection.text()

        for extractor_item in self.ui.extractor_list.selectedItems():
            for path, relative_path in self.ui.file_tree.selected_files():
                extractor = self.ui.extractor_list.itemWidget(extractor_item).item

                job_item = JobTreeItem(extractor, path, os.path.join(output_path, relative_path))
                self.ui.job_list.addTopLevelItem(job_item)

    def register_extractor(self, extractor):
        """
        Add an extractor which extracts output data from an input log file to the extractor list,
        the tooltip is set to the doc string of the class

        :param extractor
        """
        if extractor.name is None:
            extractor.name = extractor.__class__.__name__

        container = ItemContainer(extractor.name, extractor)

        list_item = QListWidgetItem(self.ui.extractor_list)
        list_item.setToolTip(extractor.__doc__)
        self.ui.extractor_list.setItemWidget(list_item, container)

        logger.debug(f'Registered extractor "{extractor.name}".')
