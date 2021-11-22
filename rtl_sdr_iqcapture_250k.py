#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Store IQ
# GNU Radio version: 3.8.1.0

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import osmosdr
import time
import os
from threading import Thread

class mainprog(gr.top_block):

    def __init__(self, fname='iq_bb_250k.bin'):
        gr.top_block.__init__(self, "Store IQ")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 250e3
        self.rf_gain = rf_gain = 20
        self.rf_bw = rf_bw = 25e3
        self.if_gain = if_gain = 20
        self.freq = freq = 437.5e6
        self.bb_gain = bb_gain = 20

        ##################################################
        # Blocks
        ##################################################
        self.rtlsdr_source_0 = osmosdr.source(
            args="numchan=" + str(1) + " " + ""
        )
        self.rtlsdr_source_0.set_time_now(osmosdr.time_spec_t(time.time()), osmosdr.ALL_MBOARDS)
        self.rtlsdr_source_0.set_sample_rate(samp_rate)
        self.rtlsdr_source_0.set_center_freq(freq, 0)
        self.rtlsdr_source_0.set_freq_corr(0, 0)
        self.rtlsdr_source_0.set_gain(rf_gain, 0)
        self.rtlsdr_source_0.set_if_gain(if_gain, 0)
        self.rtlsdr_source_0.set_bb_gain(bb_gain, 0)
        self.rtlsdr_source_0.set_antenna('', 0)
        self.rtlsdr_source_0.set_bandwidth(rf_bw, 0)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_gr_complex*1, fname, False)
        self.blocks_file_sink_0.set_unbuffered(True)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.rtlsdr_source_0, 0), (self.blocks_file_sink_0, 0))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.rtlsdr_source_0.set_sample_rate(self.samp_rate)

    def get_rf_gain(self):
        return self.rf_gain

    def set_rf_gain(self, rf_gain):
        self.rf_gain = rf_gain
        self.rtlsdr_source_0.set_gain(self.rf_gain, 0)

    def get_rf_bw(self):
        return self.rf_bw

    def set_rf_bw(self, rf_bw):
        self.rf_bw = rf_bw
        self.rtlsdr_source_0.set_bandwidth(self.rf_bw, 0)

    def get_if_gain(self):
        return self.if_gain

    def set_if_gain(self, if_gain):
        self.if_gain = if_gain
        self.rtlsdr_source_0.set_if_gain(self.if_gain, 0)

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.rtlsdr_source_0.set_center_freq(self.freq, 0)

    def get_bb_gain(self):
        return self.bb_gain

    def set_bb_gain(self, bb_gain):
        self.bb_gain = bb_gain
        self.rtlsdr_source_0.set_bb_gain(self.bb_gain, 0)



def main(top_block_cls=mainprog, options=None, fname = 'iq_bb_250k.bin'):
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Error: failed to enable real-time scheduling.")
    tb = top_block_cls(fname)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        # sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()
    tb.wait()

def cause_exit(t = 1):
    time.sleep(t)
    os.kill(os.getpid(), signal.SIGINT)


if __name__ == '__main__':
    if (len(sys.argv) != 3):
        print("Invocation: %s <Time to exit> <Output File>\n\n"%(sys.argv[0]))
        sys.exit(0)
    try:
        t = int(sys.argv[1])
    except Exception:
        print("%s is not valid time"%(sys.argv[1]))
        sys.exit(0)

    t = 5 if t < 1 else t + 5
    thread = Thread(target = cause_exit, args = (t, ))
    thread.start()
    main(fname = sys.argv[2])
    thread.join()
    print("Exiting")
    sys.exit(0)