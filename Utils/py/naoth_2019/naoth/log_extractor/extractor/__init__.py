class Extractor:
    name = None

    def extract(self, target_log, output_path, *args, **kwargs):
        raise NotImplementedError()