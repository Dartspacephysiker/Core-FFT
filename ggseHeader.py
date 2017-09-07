'''
Created on Dec 30, 2009

@author: wibble
'''

import StringIO
import ConfigParser as confp
import subprocess
import os
# Spence added 2017/09/06 to try to figure out Dartmouth header stuff in
# Rx-DSP data
from bitstring import ConstBitStream

if __name__ == '__main__':
    pass


def cpSectionCopy(orig, osect, new, nsect):
    for option in orig.options(osect):
        new.set(nsect, option, orig.get(osect, option))

    return 0


def ggseHeader(type='raw'):
    '''Read default header from file ('Constructor').'''
    defh = confp.ConfigParser()
    head = confp.ConfigParser()

    defh.read(['./default.head', '../lib/default.head',
               '/usr/ggse/lib/default.head'])

    head.add_section("ggse_header")
    defsect = 'ggse_default_header_' + type

    cpSectionCopy(defh, 'default', head, 'ggse_header')
    cpSectionCopy(defh, defsect, head, 'ggse_header')

    return head


def fhf(file):
    '''Read header from header-only file.'''
    head = confp.ConfigParser()
    head.readfp(file)

    return head


def fs(string):
    '''Read header from string.'''
    strhead = StringIO.StringIO(string.rstrip('\0'))
    strhead.flush()

    return fhf(strhead)


def ff(file, mode='unknown'):
    '''Read header from data file.'''
    infile = open(file, 'r')

    oldheader = infile.read(8)

    # Finds "Dartmouth"
    s = ConstBitStream(filename=file)
    found = s.find('0x446172746d6f757468', bytealigned=True)
    if found:
        print("Found start code at byte offset %d." % found[0])
        # s0f0, length, bitdepth, height, width = s.readlist('hex: 16, uint: 16,
        # uint: 8, 2 * uint: 16')
        # print("Width %d, Height %d" % (width, height))
    else:
        print("No way!!")

    if oldheader[0:6] == "999999" and mode == 'unknown':
        if oldheader[6:8] == "01":
            mode = 'head'
        elif oldheader[6:8] == "04":
            mode = 'tail'
        else:
            print "ff(): Mode not defined in input or old header"
            return(1)
    elif oldheader[0:6] != "999999" and mode != 'tail':
        print "ff(): No valid GGSE header signature."
        return(1)

    if mode == 'head':
        header = infile.read(8184)
    elif mode == 'tail':
        infile.seek(-8192, os.SEEK_END)
        header = infile.read(8192)
    else:
        print "ff(): Unknown mode."
        return(1)

    infile.close()

    try:
        fhead = fs(header)
    except confp.ParsingError or confp.MissingSectionHeaderError:
        print "ff(): error parsing file: No header in " + mode + " of file?"
        fhead = 1

    return fhead


def thf(file, cheader):
    '''Write header to header-only file.'''
    ftype = type(file).__name__
    if ftype == 'file' or ftype == 'instance':
        cheader.write(file)

    elif ftype == 'str':
        try:
            ofile = open(file, 'w')
        except:
            print "thf(): could not open " + str(file) + "to write header."
            return(1)

        cheader.write(ofile)
        ofile.close()
    else:
        print "thf(): what is this " + str(file) + " you speak of?"

    return 0


def ts(header):
    '''Write header to string.'''
    strhead = StringIO.StringIO()
    thf(strhead, header)
    return strhead.getvalue()


def tf(file, cheader, mode='prepend', replace=True):
    '''Write header to file, possibly a file that already has data in it.'''

    if mode == 'prepend':
        # because, for whatever reason, no OS/filesystem has the ability
        # to prepend to a file, we need to write the header to a temp file,
        # use 'cat' or 'tail' to append the data to the header, and then
        # delete the data file and rename the temp to the original name.
        sth = "99999999" + ts(cheader)
        sth += chr(0) * (8192 - len(sth))  # null-pad to standard header length

        tfn = '.ggsetmp.' + file
        tf = open(tfn, 'w')
        tf.write(sth)
        tf.close()
        if replace:
            cmd = 'tail -c +8193 ' + file + ' >> ' + tfn
        else:
            cmd = 'cat ' + file + ' >> ' + tfn
        subprocess.Popen(cmd)
        tfstat = os.stat(tfn)
        ofstat = os.stat(file)
        if tfstat.st_size() == ofstat.st_size() + len(sth):
            os.remove(file)
            os.rename(tfn, file)
        else:
            print "tf(): cat failed."

    elif mode == 'append':
        sth = ts(cheader)
        sth += chr(0) * (8192 - len(sth))  # null-pad

        if replace:
            print file
            af = open(file, 'r+b')
            af.seek(-8192, os.SEEK_END)
        else:
            af = open(file, 'a')
        af.write(sth)
        af.close()

    elif mode == 'file':
        # Yes, it's the same as append.  You could make this a 'w+', and
        # so overwrite any existing file...meh.
        nf = open(file, 'a')
        nf.write(sth)
        nf.close()

    else:
        print "tf(): Unknown mode."
        return 1

    return 0
