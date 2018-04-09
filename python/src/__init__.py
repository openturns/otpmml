"""
    otpmml --- An OpenTURNS module
    ==================================

    Contents
    --------
      'otpmml' is a module for OpenTURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

from .otpmml import *

__version__ = '1.4'
