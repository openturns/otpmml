#! /usr/bin/env python

import openturns as ot
import otpmml

# Read pmml file exported by uranie
neuralNetwork = otpmml.NeuralNetwork("uranie_ann_poutre.pmml")

# print the function
print "NeuralNetwork=", neuralNetwork

# Comparison of neuralNetwork with the original function
poutre = ot.NumericalMathFunction(["E", "F", "L", "I"],["F*L^3/(3*E*I)"])
print "poutre=" , poutre

# Import data used for comparison
samples = otpmml.DAT.Import("input_output.dat")
print samples
input_sample = samples[0]
output_sample = samples[1]

print "output sample=" , output_sample
print "NeuralNetwork output=", neuralNetwork(input_sample)

# Compare gradient & hessian
for x in input_sample:
    print "x=%s, model gradient=%s, model hessian=%s"%(x, poutre.gradient(x), poutre.hessian(x))
    print "x=%s, neural gradient=%s, neural hessian=%s"%(x, neuralNetwork.gradient(x), neuralNetwork.hessian(x))

