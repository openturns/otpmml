#! /usr/bin/env python

from __future__ import print_function
import otpmml

model = otpmml.RegressionModel("linear_regression.pmml")
least_squares = model.getLinearLeastSquares()
print("Linear regression:")
print("  constant=", least_squares.getConstant())
print("  linear=", least_squares.getLinear())

