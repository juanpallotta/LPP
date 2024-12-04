

function [alphaA, betaA, betaT] = Fernald_1983(r, attB, alfaM, betaM, nRef, betaAerRef, indxAnalysis, LR, invDirection, refMode)

  LRM = 8*pi/3;

  betaT  = zeros(length(attB), 1) ;
  alphaA = zeros(length(attB), 1) ;
  betaA  = zeros(length(attB), 1) ;

  if ( strcmp(refMode, 'fitted') ==1 )
    attBn = attB/attB(nRef) ;
  else
    refHalfSpamNorm = 20 ;
    attBn = attB / mean( attB( (nRef-refHalfSpamNorm) : (nRef+refHalfSpamNorm) ) ) ;
  endif

% ITS AN INTEGRAL WHICH USES THE PREVIOUS INDEX AS REFERENCE. IT IS NOT REFERENCE ALWAYS TO nRef

% FORWARD INVERSION - FERNALD 1983
if ( strcmp(invDirection, 'forward') ==1 || (strcmp(invDirection, 'both') ==1))
    betaA(nRef-1) = betaAerRef ; % attB(nRef) / exp(-2 * trapz( r(indxAnalysis(1):nRef), alfaM(indxAnalysis(1):nRef) ) ) -betaM(nRef) ;
    dz = r(3)-r(2) ;
    for i=nRef:indxAnalysis(end)
      phi = (LR-LRM) * ( betaM(i-1) + betaM(i) ) *dz ; % dz >0 --> phi >0
      p   = attBn(i) * exp(-phi);

      CalTerm = attBn(i-1) / (betaA(i-1) + betaM(i-1) ) ;
      ip =  ( attBn(i-1) + p ) *dz /2 ;

      betaT = p ./ ( CalTerm - 2*LR * ip ) ;
      betaA(i) = betaT - betaM(i) ;
      alphaA(i) = betaA(i) * LR ;
    endfor
endif
% BACKWARD INVERSION - FERNALD 1983
if ( (strcmp(invDirection, 'backward') ==1) || (strcmp(invDirection, 'both') ==1) )
  betaA(nRef+1) = betaAerRef ; % attB(nRef) / exp(-2 * trapz( r(indxAnalysis(1):nRef), alfaM(indxAnalysis(1):nRef) ) ) -betaM(nRef) ;
  dz = r(2)-r(3) ;
  for i=nRef:-1:indxAnalysis(2)
    phi = (LR-LRM) * ( betaM(i+1) + betaM(i) ) *dz ; % dz <0 --> phi <0 IN THE NEXT LINE
    p   = attBn(i) * exp(-phi); % attBn(i) * exp(phi);

    CalTerm = attBn(i+1) / (betaA(i+1) + betaM(i+1) ) ;
    ip = ( attBn(i+1) + p ) *dz /2 ;

    betaT = p / ( CalTerm - 2*LR * ip ) ;
    betaA(i) = betaT - betaM(i) ;
    alphaA(i) = betaA(i) * LR ;
  endfor
endif

endfunction