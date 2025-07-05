import numpy as np
import matplotlib.pyplot as plt

class circuito:
    def __init__(self, begin, end, dt, capacitor, resistors, voltagesource, currentsource):
        self.time = np.arange(begin, end, dt)
        self.dt = dt
        self.capacitor = capacitor
        self.resistors = resistors
        self.voltagesource = voltagesource
        self.currentsource = currentsource
        self.vf_charge = np.zeros_like(self.time)


    def simular(self):
        # Variaveis observáveis
        vc_charge = np.zeros_like(self.time) # Tensão no capacitor ao longo do tempo0
        i2_charge = np.zeros_like(self.time) # Corrente i2 ao longo do tempo

        ##CARGA
        # Calculo inicial de i2 e vf
        i2_charge[0] = (self.voltagesource[0] + (self.resistors[2] * self.currentsource) - self.voltagesource[1]) / (self.resistors[1] + self.resistors[2])
        self.vf_charge[0] = (self.currentsource * (self.resistors[0] + self.resistors[2]) - self.resistors[2] * i2_charge[0] + self.voltagesource[0]) * 6

        # Calculando a tensão carga
        for i in range(1, len(self.time)):
            # Calculo de tensão no resistor
            deltavc = ((i2_charge[i - 1] / self.capacitor) - (vc_charge[i - 1] / (self.resistors[3] * self.capacitor))) * self.dt
            vc_charge[i] = vc_charge[i - 1] + deltavc

            # Cálculo de corrente
            i2_charge[i] = (self.voltagesource[0] + (self.resistors[2] * self.currentsource) - vc_charge[i] - self.voltagesource[1]) / (self.resistors[1] + self.resistors[2])

            # Cálculo da tensão na fonte
            self.vf_charge[i] = (self.currentsource * (self.resistors[0] + self.resistors[2]) - self.resistors[2] * i2_charge[i] + self.voltagesource[0]) * 6

        self.vf_charge = self.vf_charge
