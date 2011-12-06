#!/usr/bin/env python
import ROOT
import avalanche

# create client listening to localhost port 7777
cli = avalanche.Client('tcp://localhost:7777')

# receive a TH1F and print the integral
h = cli.recv_object(ROOT.TH1F.Class())
print h.Integral()

