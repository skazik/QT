import logging
from threading import Lock


class Logger:
    _instance = None
    _lock = Lock()

    def __new__(cls):
        """Implement singleton pattern with thread safety."""
        if not cls._instance:
            with cls._lock:
                if not cls._instance:
                    cls._instance = super().__new__(cls)
                    cls._instance._initialized = False
        return cls._instance

    def __init__(self):
        """Initialize the logger configuration only once."""
        if self._initialized:
            return

        # Set up the logger
        self.logger = logging.getLogger("ApplicationLogger")
        self.set_debug_mode(False)  # must be non-debug mode before commit to git
        self.console_handler = logging.StreamHandler()
        self.logger.addHandler(self.console_handler)

        # Prevent duplicate log entries
        self.logger.propagate = False

        self._timestamp_enabled = False  # Timestamps are disbled by default
        self._update_formatter()

        # Mark initialization as complete
        self._initialized = True

    def _update_formatter(self):
        """Update the logging format based on the timestamp setting."""
        if self._timestamp_enabled:
            formatter = logging.Formatter("%(asctime)s - %(levelname)s: %(message)s")
        else:
            formatter = logging.Formatter("%(levelname)s: %(message)s")

        self.console_handler.setFormatter(formatter)

    def set_debug_mode(self, debug=True):
        """Enable or disable debug mode."""
        if debug:
            self.logger.setLevel(logging.DEBUG)
        else:
            self.logger.setLevel(logging.INFO)

    def debug(self, message):
        """Log a debug message."""
        self.logger.debug(message)

    def info(self, message):
        """Log an info message."""
        self.logger.info(message)

    def warning(self, message):
        """Log a warning message."""
        self.logger.warning(message)

    def error(self, message):
        """Log an error message."""
        self.logger.error(message)

    def critical(self, message):
        """Log a critical message."""
        self.logger.critical(message)
