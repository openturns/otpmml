#! /usr/bin/env python

import openturns as ot
import otpmml

# Comparison of neuralNetwork with the original function
poutre = ot.SymbolicFunction(["E", "F", "L", "I"], ["F*L^3/(3*E*I)"])
print("poutre=", repr(poutre))

# Import data used for comparison
samples = otpmml.DAT.Import("input_output.dat")
input_sample = samples[0]
output_sample = samples[1]

print("input sample=", input_sample)
print("output sample=", output_sample)
result = poutre(input_sample)

print("Analytical output=", result)

relative_difference = [0] * result.getSize()
for i in range(result.getSize()):
    relative_difference.append(
        abs((result[i][0] - output_sample[i][0]) / result[i][0]))

if max(relative_difference) > 1.e-10:
    print("Relative difference=", relative_difference)

otpmml.DAT.Export("export.dat", input_sample, output_sample)
