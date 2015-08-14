#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otpmml

# Read pmml file exported by uranie
neuralNetwork = otpmml.NeuralNetwork("uranie_ann_poutre.pmml")

# Import data used for comparison
samples = otpmml.DAT.Import("input_output.dat")
input_sample = samples[0]
output_sample = samples[1]
nnet_sample = neuralNetwork(input_sample)
diff_sample = nnet_sample - output_sample
result_sample = ot.NumericalSample(input_sample)
result_sample.stack(output_sample)
result_sample.stack(nnet_sample)
result_sample.stack(diff_sample)
result_sample.setDescription(ot.Description(["E", "F", "L", "I", "expected", "nnet", "difference"]))
print(result_sample)

# Comparison of neuralNetwork with the original function
poutre = ot.NumericalMathFunction(["E", "F", "L", "I"],["F*L^3/(3*E*I)"])
print("poutre=" , repr(poutre))

# Compare gradient & hessian
for x in input_sample:
    print("x=%s, model gradient=%s, neural gradient=%s"%(x, poutre.gradient(x), neuralNetwork.gradient(x)))
    print("x=%s, model hessian=%s, neural hessian=%s"%(x, poutre.hessian(x), neuralNetwork.hessian(x)))
