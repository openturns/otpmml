import openturns as ot
import modulePMML
import otpmml

inputs, outputs = tuple(otpmml.DAT.Import("input_output.dat"))
size = len(inputs)

model = modulePMML.monModelePMML("uranie_ann_poutre.pmml")
pmmlRef = ot.NumericalMathFunction(model)
pmmlOT = otpmml.NeuralNetwork("uranie_ann_poutre.pmml")
eval_module = pmmlRef(inputs)
eval_otpmml = pmmlOT(inputs)

results = ot.NumericalSample(0, 4)
description = ["modulePMML", "OTPMML", "abs. error", "rel. error"]
results.setDescription(description)
for i in xrange(size):
    point = ot.NumericalPoint(inputs[i])
    results.add([ \
          eval_module[i][0], \
          eval_otpmml[i][0], \
          abs(eval_module[i][0] - eval_otpmml[i][0]), \
          abs((eval_module[i][0] - eval_otpmml[i][0])/eval_module[i][0])  ])
print results

