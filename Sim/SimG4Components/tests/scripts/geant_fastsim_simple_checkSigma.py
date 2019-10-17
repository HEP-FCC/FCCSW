import ROOT

rfile = ROOT.TFile.Open("./histSimple.root", "READ")
rfile.ls()
diff = rfile.Get("DiffP")
params = diff.Fit("gaus","S")
constant = params.Parameter(0)
mean = params.Parameter(1)
sigma = params.Parameter(2)
errconstant = params.Error(0)
errmean = params.Error(1)
errsigma = params.Error(2)

assert(-10e-3 < mean-errmean < 10e-3)
assert(0.007 < sigma-errsigma < 0.013)
