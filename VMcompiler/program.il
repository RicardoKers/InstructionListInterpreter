# operações de entrada e saída com Bits
# LD IX0.0 
# ST QX0.0
# STN QX0.1

# LDN IX0.1
# S QX0.2
# R QX0.1

# Testando Operações Lógicas
# LD IX0.0
# AND IX0.1
# OR IX0.2
# ST QX0.0

# LD IX0.0
# AND( IX0.1
# OR IX0.2
# )
# ST QX0.1 

# LD IX0.0
# ANDN IX0.1
# ST QX0.0 

# Testando MOV
# LD KX1
# ST QX0.0
# MOV QX0.0 MX0.0

# LD KX1
# MOV KW0x1A2B MW0
# MOV MW0 QW2

# LD KX1 
# MOV IB0 QB0
# MOV IB1 QB1

# Testando ANDN e ANDN(
# LD IX0.0
# ANDN IX0.1
# OR IX0.2 
# ST QX0.0 

# LD IX0.0
# ANDN( IX0.1
# OR IX0.2
# ) 
# ST QX0.1 

# Testando Soma
# LD IX0.0
# MOV QW0 MW0
# ADD KW1 MW0 MW0
# MOV MW0 QW0

# LD KX1
# MOD KW10 IW0 QW0

LD KX1
# CTU(ncouter, CO, PV, RST, OUT, CV);
CTU KB3 IX0.0 KW4 IX0.1 MX1.0 MW0
MOV MW0 QW0 