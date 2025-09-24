# PowerDistributionUnit_TPS25983

This board was designed in the initial attempt towards a smart power distribution unit in the 2024-2025 school year by Changxu Liu and Wataru (Watts) Yasueda. 

It was abandoned due to to multiple issues, primarily in the selection of the TPS25983 eFuse:
- The QFN package was very difficult to solder, requiring a hot air station due to unexposed pads. This would only be worse if repairs had to be made when competing. 
- The eFuse requires a large number of additional passive components. These exist to support all the configurable settings the chip provides (ex. slew rate control, overcurrent blanking, configurable current limit, ...) which were ultimately found to be not necessary and unused - leaving them at arbitrary values. 

This board has never been ordered or validated. Attention was shifted towards a next iteration utilizing the TPS27SA08-Q1 high-side switch instead. 
