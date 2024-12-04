// Lidar system of units
//

#ifndef __utl_LidarSystemOfUnits_h__
#define __utl_LidarSystemOfUnits_h__

namespace utl {
    
  /**
     \class LidarUnits

     \brief Defines units in terms of Lidar standard units

     You should use the units defined in this file whenever you have a 
     dimensional quantity in your code.  For example, write:  
     \code  
      double s = 1.5*km; 
     \endcode   
     instead of:
     \code
       double s = 1.5;   // don't forget this is in km!
     \endcode
     The conversion factors defined in this file convert your data into Lidar 
     base units, so that all dimensional quantities in the code are in a
     single system of units!  You can also use the conversions defined here to,
     for example, display data with the unit of your choice.  For example:
     \code
       cout << "s = " << s / mm << " mm";
     \endcode
     The base units are : 
    - meter                     (meter)
    - nanosecond                (nanosecond)
    - ampere                    (A)
    - degree Kelvin             (kelvin)
    - the amount of substance   (mole)
    - luminous intensity        (candela)
    - radian                    (radian)
    - steradian                 (steradian)

     Below is a non-exhaustive list of derived and pratical units
     (i.e. mostly the SI units).
  
     The SI numerical value of the positron charge is defined here,
     as it is needed for conversion factor : positron charge = eSI (coulomb)
  */

  // ---------------------------------------------------------------------------
  // Standard metric prefixes.
  // ---------------------------------------------------------------------------
  const double kilo_  = 1.e+03;
  const double mega_  = 1.e+06;
  const double giga_  = 1.e+09;
  const double tera_  = 1.e+12;
  const double peta_  = 1.e+15;
  const double exa_   = 1.e+18;
  const double zetta_ = 1.e+21;

  const double deci_  = 1.e-01;
  const double centi_ = 1.e-02;
  const double milli_ = 1.e-03;
  const double micro_ = 1.e-06;
  const double nano_  = 1.e-09;
  const double pico_  = 1.e-12;

  // ---------------------------------------------------------------------------
  // Base (default) SI units for basic measurable quantities, with symbols.
  // ---------------------------------------------------------------------------
  // Length
  const double meter = 1.;
  const double m = meter;

  // Mass
  const double kilogram = 1.;
  const double kg = kilogram;

  // Time [default is ns]
  const double second = 1.; // 1.e+09;
  const double s = second;

  // Current
  const double ampere = 1.;
  const double A = ampere;

  // Temperature
  const double kelvin = 1.;
  const double K = kelvin;

  // Amount of substance
  const double mole = 1.;
  const double mol = mole;

  // Luminous intensity
  const double candela = 1.;
  const double cd = candela;
  
  // ---------------------------------------------------------------------------
  // Supplementary SI units, with symbols.
  // ---------------------------------------------------------------------------
  const double radian      = 1.;                  
  const double rad         = radian;
  const double milliradian = milli_ * radian;
  const double mrad        = milliradian;

  const double pi          = 3.1415926535897932384626;
  const double degree      = radian * pi / 180.;
  const double deg         = degree;

  const double steradian   = 1.;
  const double sr          = steradian;
  
  // ---------------------------------------------------------------------------
  //  Derived SI units, with symbols.
  // ---------------------------------------------------------------------------
  // Frequency
  const double hertz = 1. / second;
  const double Hz = hertz;

  // Force
  const double newton = meter * kilogram / (second * second);
  const double N = newton;

  // Pressure
  const double pascal = newton / (meter * meter);
  const double atmosphere = 101325. * pascal;
  const double bar = 100000 * pascal;
  const double Pa = pascal;
  const double atm = atmosphere;
  
  // Energy
  const double joule = newton * meter;
  const double J = joule;

  // Power
  const double watt = joule / second;
  const double W = watt;

  // Electric charge
  const double coulomb = ampere * second;
  const double C = coulomb;

  // Electric potential
  const double volt = joule / coulomb;
  const double V = volt;

  // Electric resistance
  const double ohm = volt / ampere;

  // Electric conductance
  const double siemens = 1. / ohm;
  const double S = siemens;

  // Electric capacitance
  const double farad = coulomb / volt;
  const double F = farad;

  // Magnetic flux density
  const double tesla = volt * second / (meter * meter);
  const double gauss = 1.e-4 * tesla;
  const double T = tesla;
  const double Gs = gauss;

  // Magnetic flux
  const double weber = volt * second;
  const double Wb = weber;
  
  // Inductance
  const double henry = weber / ampere;
  const double H = henry;

  // Luminous flux
  const double lumen = candela * steradian;
  const double lm = lumen;

  // Illuminance
  const double lux = lumen / (meter * meter);
  const double lx = lux;

  // Radioactivity
  const double becquerel = 1. / second;
  const double curie = 3.7e+10 * becquerel;
  const double Bq = becquerel;
  const double Ci = curie;

  // Specific energy
  const double gray = joule / kilogram;
  const double Gy = gray;

  // Dose equivalent
  const double sievert = joule / kilogram;
  const double Sv = sievert;

  // ---------------------------------------------------------------------------
  // Further selected units, with symbols.
  // ---------------------------------------------------------------------------
  
  //
  // Length [L]
  //
  const double millimeter  = milli_ * meter;                        
  const double mm          = millimeter;                        
  const double millimeter2 = millimeter * millimeter;
  const double mm2         = millimeter2;
  const double millimeter3 = millimeter * millimeter * millimeter;
  const double mm3         = millimeter3;
  
  const double centimeter  = centi_ * meter;   
  const double cm          = centimeter;   
  const double centimeter2 = centimeter * centimeter;
  const double cm2         = centimeter2;
  const double centimeter3 = centimeter * centimeter * centimeter;
  const double cm3         = centimeter3;
  
  const double meter2 = meter * meter;
  const double m2     = meter2;
  const double meter3 = meter * meter * meter;
  const double m3     = meter3;
  
  const double kilometer  = kilo_ * meter;                   
  const double km         = kilometer;                   
  const double kilometer2 = kilometer * kilometer;
  const double km2        = kilometer2;
  const double kilometer3 = kilometer * kilometer * kilometer;
  const double km3        = kilometer3;
  
  const double micrometer = micro_ * meter;             
  const double micron     = micrometer;
  const double nanometer  = nano_ * meter;
  const double nm         = nanometer;
  const double angstrom   = 1.e-10 * meter;
  const double fermi      = 1.e-15 * meter;
  
  const double barn      = 1.e-28 * meter2;
  const double millibarn = milli_ * barn;
  const double microbarn = micro_ * barn;
  const double nanobarn  = nano_ * barn;
  const double picobarn  = pico_ * barn;
  
  
  //
  // Time [T]
  //
  const double millisecond = milli_ * second;
  const double ms          = millisecond;
  const double microsecond = micro_ * second;
  const double us          = microsecond;
  const double nanosecond  = nano_ * second;
  const double ns          = nanosecond;
  const double picosecond  = pico_ * second;
  const double ps          = picosecond;

  const double minute      = 60. * second;
  const double hour        = 60. * minute;
  const double day         = 24. * hour;

  const double kilohertz   = kilo_ * hertz;
  const double kHz         = kilohertz;
  const double megahertz   = mega_ * hertz;
  const double MHz         = megahertz;
  const double gigahertz   = giga_ * hertz;
  const double GHz         = gigahertz;
  
  //
  // Electric current [Q][T^-1]
  //
  const double milliampere = milli_ * ampere;
  const double mA          = milliampere;
  const double microampere = micro_ * ampere;
  const double uA          = microampere;
  const double nanoampere  = nano_ * ampere;
  const double nA          = nanoampere;

  //
  // Electric potential [E][Q^-1]
  //
  const double millivolt = milli_ * volt;
  const double mV        = millivolt;
  const double kilovolt  = kilo_ * volt;
  const double kV        = kilovolt;
  const double megavolt  = mega_ * volt;
  const double MV        = megavolt;

  //
  // Energy [E]
  //
  const double microjoule = micro_ * joule;
  const double uJ         = microjoule;
  const double millijoule = micro_ * joule;
  const double mJ         = millijoule;
  const double kilojoule  = kilo_ * joule;
  const double kJ         = kilojoule;
  const double megajoule  = mega_ * joule;
  const double MJ         = megajoule;
  
  const double eSI   = 1.60217733e-19;   // positron charge in coulomb
  const double eplus = eSI * coulomb;    // positron charge

  const double electronvolt      = eplus * volt;
  const double eV                = electronvolt;
  const double kiloelectronvolt  = kilo_ * electronvolt;
  const double keV               = kiloelectronvolt;
  const double megaelectronvolt  = mega_ * electronvolt;
  const double MeV               = megaelectronvolt;
  const double gigaelectronvolt  = giga_ * electronvolt;
  const double GeV               = gigaelectronvolt;
  const double teraelectronvolt  = tera_ * electronvolt;
  const double TeV               = teraelectronvolt;
  const double petaelectronvolt  = peta_ * electronvolt;
  const double PeV               = petaelectronvolt;
  const double exaelectronvolt   = exa_ * electronvolt;
  const double EeV               = exaelectronvolt;
  const double zettaelectronvolt = zetta_ * electronvolt;
  const double ZeV               = zettaelectronvolt;
  
  //
  // Mass [E][T^2][L^-2]
  //
  const double gram      = 1.e-3 * kilogram;
  const double g         = gram;
  const double milligram = milli_ * gram;
  const double mg        = milligram;
  
  //
  // Electric capacitance [Q^2][E^-1]
  //
  const double millifarad = milli_ * farad;
  const double mF         = millifarad;
  const double microfarad = micro_ * farad;
  const double uF         = microfarad;
  const double nanofarad  = nano_ * farad;
  const double nF         = nanofarad;
  const double picofarad  = pico_ * farad;
  const double pF         = picofarad;
  
  //
  // Miscellaneous
  //
  const double perCent     = 0.01;
  const double percent     = perCent;
  const double perThousand = 0.001;
  const double permil      = perThousand;
  const double perMillion  = 0.000001;
  
} // End namespace utl

#endif // __utl_LidarSystemOfUnits_h__

