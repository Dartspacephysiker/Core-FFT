# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.2
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_coreFFT', [dirname(__file__)])
        except ImportError:
            import _coreFFT
            return _coreFFT
        if fp is not None:
            try:
                _mod = imp.load_module('_coreFFT', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _coreFFT = swig_import_helper()
    del swig_import_helper
else:
    import _coreFFT
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0



def complex_1chan(*args):
  return _coreFFT.complex_1chan(*args)
complex_1chan = _coreFFT.complex_1chan

def uint16_1chan(*args):
  return _coreFFT.uint16_1chan(*args)
uint16_1chan = _coreFFT.uint16_1chan

def float4_1chan(*args):
  return _coreFFT.float4_1chan(*args)
float4_1chan = _coreFFT.float4_1chan

def float8_1chan(*args):
  return _coreFFT.float8_1chan(*args)
float8_1chan = _coreFFT.float8_1chan

def real_nchan(*args):
  return _coreFFT.real_nchan(*args)
real_nchan = _coreFFT.real_nchan

def uint8_1chan(*args):
  return _coreFFT.uint8_1chan(*args)
uint8_1chan = _coreFFT.uint8_1chan
_FILE_OFFSET_BITS = _coreFFT._FILE_OFFSET_BITS
CORE_MAXCHAN = _coreFFT.CORE_MAXCHAN
N_THREADS = _coreFFT.N_THREADS
N_PHASES = _coreFFT.N_PHASES
CORE_RFP = _coreFFT.CORE_RFP
COREH_NONE = _coreFFT.COREH_NONE
COREH_HEAD = _coreFFT.COREH_HEAD
COREH_TAIL = _coreFFT.COREH_TAIL
PI = _coreFFT.PI
PITT = _coreFFT.PITT
class polynomial_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, polynomial_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, polynomial_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["coefficients"] = _coreFFT.polynomial_t_coefficients_set
    __swig_getmethods__["coefficients"] = _coreFFT.polynomial_t_coefficients_get
    if _newclass:coefficients = _swig_property(_coreFFT.polynomial_t_coefficients_get, _coreFFT.polynomial_t_coefficients_set)
    __swig_setmethods__["expansion_origin"] = _coreFFT.polynomial_t_expansion_origin_set
    __swig_getmethods__["expansion_origin"] = _coreFFT.polynomial_t_expansion_origin_get
    if _newclass:expansion_origin = _swig_property(_coreFFT.polynomial_t_expansion_origin_get, _coreFFT.polynomial_t_expansion_origin_set)
    __swig_setmethods__["order"] = _coreFFT.polynomial_t_order_set
    __swig_getmethods__["order"] = _coreFFT.polynomial_t_order_get
    if _newclass:order = _swig_property(_coreFFT.polynomial_t_order_get, _coreFFT.polynomial_t_order_set)
    def __init__(self): 
        this = _coreFFT.new_polynomial_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _coreFFT.delete_polynomial_t
    __del__ = lambda self : None;
polynomial_t_swigregister = _coreFFT.polynomial_t_swigregister
polynomial_t_swigregister(polynomial_t)
cvar = _coreFFT.cvar

class core_param(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, core_param, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, core_param, name)
    __repr__ = _swig_repr
    __swig_setmethods__["infile"] = _coreFFT.core_param_infile_set
    __swig_getmethods__["infile"] = _coreFFT.core_param_infile_get
    if _newclass:infile = _swig_property(_coreFFT.core_param_infile_get, _coreFFT.core_param_infile_set)
    __swig_setmethods__["outfile"] = _coreFFT.core_param_outfile_set
    __swig_getmethods__["outfile"] = _coreFFT.core_param_outfile_get
    if _newclass:outfile = _swig_property(_coreFFT.core_param_outfile_get, _coreFFT.core_param_outfile_set)
    __swig_setmethods__["timefile"] = _coreFFT.core_param_timefile_set
    __swig_getmethods__["timefile"] = _coreFFT.core_param_timefile_get
    if _newclass:timefile = _swig_property(_coreFFT.core_param_timefile_get, _coreFFT.core_param_timefile_set)
    __swig_setmethods__["N"] = _coreFFT.core_param_N_set
    __swig_getmethods__["N"] = _coreFFT.core_param_N_get
    if _newclass:N = _swig_property(_coreFFT.core_param_N_get, _coreFFT.core_param_N_set)
    __swig_setmethods__["windiv"] = _coreFFT.core_param_windiv_set
    __swig_getmethods__["windiv"] = _coreFFT.core_param_windiv_get
    if _newclass:windiv = _swig_property(_coreFFT.core_param_windiv_get, _coreFFT.core_param_windiv_set)
    __swig_setmethods__["overlap"] = _coreFFT.core_param_overlap_set
    __swig_getmethods__["overlap"] = _coreFFT.core_param_overlap_get
    if _newclass:overlap = _swig_property(_coreFFT.core_param_overlap_get, _coreFFT.core_param_overlap_set)
    __swig_setmethods__["skip_fft"] = _coreFFT.core_param_skip_fft_set
    __swig_getmethods__["skip_fft"] = _coreFFT.core_param_skip_fft_get
    if _newclass:skip_fft = _swig_property(_coreFFT.core_param_skip_fft_get, _coreFFT.core_param_skip_fft_set)
    __swig_setmethods__["skip_avg"] = _coreFFT.core_param_skip_avg_set
    __swig_getmethods__["skip_avg"] = _coreFFT.core_param_skip_avg_get
    if _newclass:skip_avg = _swig_property(_coreFFT.core_param_skip_avg_get, _coreFFT.core_param_skip_avg_set)
    __swig_setmethods__["avg"] = _coreFFT.core_param_avg_set
    __swig_getmethods__["avg"] = _coreFFT.core_param_avg_get
    if _newclass:avg = _swig_property(_coreFFT.core_param_avg_get, _coreFFT.core_param_avg_set)
    __swig_setmethods__["n_chan"] = _coreFFT.core_param_n_chan_set
    __swig_getmethods__["n_chan"] = _coreFFT.core_param_n_chan_get
    if _newclass:n_chan = _swig_property(_coreFFT.core_param_n_chan_get, _coreFFT.core_param_n_chan_set)
    __swig_setmethods__["n_ffts"] = _coreFFT.core_param_n_ffts_set
    __swig_getmethods__["n_ffts"] = _coreFFT.core_param_n_ffts_get
    if _newclass:n_ffts = _swig_property(_coreFFT.core_param_n_ffts_get, _coreFFT.core_param_n_ffts_set)
    __swig_setmethods__["n_startsample"] = _coreFFT.core_param_n_startsample_set
    __swig_getmethods__["n_startsample"] = _coreFFT.core_param_n_startsample_get
    if _newclass:n_startsample = _swig_property(_coreFFT.core_param_n_startsample_get, _coreFFT.core_param_n_startsample_set)
    __swig_setmethods__["bl_last"] = _coreFFT.core_param_bl_last_set
    __swig_getmethods__["bl_last"] = _coreFFT.core_param_bl_last_get
    if _newclass:bl_last = _swig_property(_coreFFT.core_param_bl_last_get, _coreFFT.core_param_bl_last_set)
    __swig_setmethods__["bl_first"] = _coreFFT.core_param_bl_first_set
    __swig_getmethods__["bl_first"] = _coreFFT.core_param_bl_first_get
    if _newclass:bl_first = _swig_property(_coreFFT.core_param_bl_first_get, _coreFFT.core_param_bl_first_set)
    __swig_setmethods__["agc_bin"] = _coreFFT.core_param_agc_bin_set
    __swig_getmethods__["agc_bin"] = _coreFFT.core_param_agc_bin_get
    if _newclass:agc_bin = _swig_property(_coreFFT.core_param_agc_bin_get, _coreFFT.core_param_agc_bin_set)
    __swig_setmethods__["agc_level"] = _coreFFT.core_param_agc_level_set
    __swig_getmethods__["agc_level"] = _coreFFT.core_param_agc_level_get
    if _newclass:agc_level = _swig_property(_coreFFT.core_param_agc_level_get, _coreFFT.core_param_agc_level_set)
    __swig_setmethods__["poly"] = _coreFFT.core_param_poly_set
    __swig_getmethods__["poly"] = _coreFFT.core_param_poly_get
    if _newclass:poly = _swig_property(_coreFFT.core_param_poly_get, _coreFFT.core_param_poly_set)
    __swig_setmethods__["frequency"] = _coreFFT.core_param_frequency_set
    __swig_getmethods__["frequency"] = _coreFFT.core_param_frequency_get
    if _newclass:frequency = _swig_property(_coreFFT.core_param_frequency_get, _coreFFT.core_param_frequency_set)
    __swig_setmethods__["time_start"] = _coreFFT.core_param_time_start_set
    __swig_getmethods__["time_start"] = _coreFFT.core_param_time_start_get
    if _newclass:time_start = _swig_property(_coreFFT.core_param_time_start_get, _coreFFT.core_param_time_start_set)
    __swig_setmethods__["time_stop"] = _coreFFT.core_param_time_stop_set
    __swig_getmethods__["time_stop"] = _coreFFT.core_param_time_stop_get
    if _newclass:time_stop = _swig_property(_coreFFT.core_param_time_stop_get, _coreFFT.core_param_time_stop_set)
    __swig_setmethods__["time_avg"] = _coreFFT.core_param_time_avg_set
    __swig_getmethods__["time_avg"] = _coreFFT.core_param_time_avg_get
    if _newclass:time_avg = _swig_property(_coreFFT.core_param_time_avg_get, _coreFFT.core_param_time_avg_set)
    __swig_setmethods__["time_nfft"] = _coreFFT.core_param_time_nfft_set
    __swig_getmethods__["time_nfft"] = _coreFFT.core_param_time_nfft_get
    if _newclass:time_nfft = _swig_property(_coreFFT.core_param_time_nfft_get, _coreFFT.core_param_time_nfft_set)
    __swig_setmethods__["time_fftmod"] = _coreFFT.core_param_time_fftmod_set
    __swig_getmethods__["time_fftmod"] = _coreFFT.core_param_time_fftmod_get
    if _newclass:time_fftmod = _swig_property(_coreFFT.core_param_time_fftmod_get, _coreFFT.core_param_time_fftmod_set)
    __swig_setmethods__["window"] = _coreFFT.core_param_window_set
    __swig_getmethods__["window"] = _coreFFT.core_param_window_get
    if _newclass:window = _swig_property(_coreFFT.core_param_window_get, _coreFFT.core_param_window_set)
    __swig_setmethods__["winpar"] = _coreFFT.core_param_winpar_set
    __swig_getmethods__["winpar"] = _coreFFT.core_param_winpar_get
    if _newclass:winpar = _swig_property(_coreFFT.core_param_winpar_get, _coreFFT.core_param_winpar_set)
    __swig_setmethods__["verbose"] = _coreFFT.core_param_verbose_set
    __swig_getmethods__["verbose"] = _coreFFT.core_param_verbose_get
    if _newclass:verbose = _swig_property(_coreFFT.core_param_verbose_get, _coreFFT.core_param_verbose_set)
    __swig_setmethods__["threads"] = _coreFFT.core_param_threads_set
    __swig_getmethods__["threads"] = _coreFFT.core_param_threads_get
    if _newclass:threads = _swig_property(_coreFFT.core_param_threads_get, _coreFFT.core_param_threads_set)
    __swig_setmethods__["convert"] = _coreFFT.core_param_convert_set
    __swig_getmethods__["convert"] = _coreFFT.core_param_convert_get
    if _newclass:convert = _swig_property(_coreFFT.core_param_convert_get, _coreFFT.core_param_convert_set)
    __swig_setmethods__["continuous"] = _coreFFT.core_param_continuous_set
    __swig_getmethods__["continuous"] = _coreFFT.core_param_continuous_get
    if _newclass:continuous = _swig_property(_coreFFT.core_param_continuous_get, _coreFFT.core_param_continuous_set)
    __swig_setmethods__["phases"] = _coreFFT.core_param_phases_set
    __swig_getmethods__["phases"] = _coreFFT.core_param_phases_get
    if _newclass:phases = _swig_property(_coreFFT.core_param_phases_get, _coreFFT.core_param_phases_set)
    __swig_setmethods__["binary"] = _coreFFT.core_param_binary_set
    __swig_getmethods__["binary"] = _coreFFT.core_param_binary_get
    if _newclass:binary = _swig_property(_coreFFT.core_param_binary_get, _coreFFT.core_param_binary_set)
    __swig_setmethods__["linear"] = _coreFFT.core_param_linear_set
    __swig_getmethods__["linear"] = _coreFFT.core_param_linear_get
    if _newclass:linear = _swig_property(_coreFFT.core_param_linear_get, _coreFFT.core_param_linear_set)
    __swig_setmethods__["oldheader"] = _coreFFT.core_param_oldheader_set
    __swig_getmethods__["oldheader"] = _coreFFT.core_param_oldheader_get
    if _newclass:oldheader = _swig_property(_coreFFT.core_param_oldheader_get, _coreFFT.core_param_oldheader_set)
    __swig_setmethods__["newheader"] = _coreFFT.core_param_newheader_set
    __swig_getmethods__["newheader"] = _coreFFT.core_param_newheader_get
    if _newclass:newheader = _swig_property(_coreFFT.core_param_newheader_get, _coreFFT.core_param_newheader_set)
    def __init__(self): 
        this = _coreFFT.new_core_param()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _coreFFT.delete_core_param
    __del__ = lambda self : None;
core_param_swigregister = _coreFFT.core_param_swigregister
core_param_swigregister(core_param)

class core_return(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, core_return, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, core_return, name)
    __repr__ = _swig_repr
    __swig_setmethods__["nfft"] = _coreFFT.core_return_nfft_set
    __swig_getmethods__["nfft"] = _coreFFT.core_return_nfft_get
    if _newclass:nfft = _swig_property(_coreFFT.core_return_nfft_get, _coreFFT.core_return_nfft_set)
    __swig_setmethods__["time_total"] = _coreFFT.core_return_time_total_set
    __swig_getmethods__["time_total"] = _coreFFT.core_return_time_total_get
    if _newclass:time_total = _swig_property(_coreFFT.core_return_time_total_get, _coreFFT.core_return_time_total_set)
    __swig_setmethods__["min"] = _coreFFT.core_return_min_set
    __swig_getmethods__["min"] = _coreFFT.core_return_min_get
    if _newclass:min = _swig_property(_coreFFT.core_return_min_get, _coreFFT.core_return_min_set)
    __swig_setmethods__["max"] = _coreFFT.core_return_max_set
    __swig_getmethods__["max"] = _coreFFT.core_return_max_get
    if _newclass:max = _swig_property(_coreFFT.core_return_max_get, _coreFFT.core_return_max_set)
    def __init__(self): 
        this = _coreFFT.new_core_return()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _coreFFT.delete_core_return
    __del__ = lambda self : None;
core_return_swigregister = _coreFFT.core_return_swigregister
core_return_swigregister(core_return)


def do_depart(*args):
  return _coreFFT.do_depart(*args)
do_depart = _coreFFT.do_depart

def window_sel(*args):
  return _coreFFT.window_sel(*args)
window_sel = _coreFFT.window_sel

def window_cog(*args):
  return _coreFFT.window_cog(*args)
window_cog = _coreFFT.window_cog

def apply_polynomial(*args):
  return _coreFFT.apply_polynomial(*args)
apply_polynomial = _coreFFT.apply_polynomial
# This file is compatible with both classic and new-style classes.


