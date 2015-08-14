#! /usr/bin/env python

import openturns as ot
import otpmml

model = otpmml.RegressionModel("linear_regression.pmml")
leastSquares = model.getLinearLeastSquares()
# print the function
print "leastSquares=", leastSquares

model.exportToPMMLFile("linear_out.pmml")
