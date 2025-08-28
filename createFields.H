#include "createRDeltaT.H"

Info<< "Reading field p_rgh\n" << endl;
volScalarField p_rgh
(
    IOobject
    (
        "p_rgh",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

/*----------------------------------------------------------*/

Info << "Reading porous media properties\n" << endl;
IOdictionary porousProps
(
    IOobject
    (
        "porousProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

// Read permeability K (dimensioned)
dimensionedScalar Kdim
(
    "K",
    dimensionSet(0, 2, 0, 0, 0, 0, 0),  // [m²]
    porousProps.subDict("porousRegion")
);
// Scalar value of K
scalar K = Kdim.value();
Info << "Porous permeability scalar K = " << K << endl;

/*----------------------------------------------------------*/

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

#include "createPhi.H"

Info<< "Reading transportProperties\n" << endl;
immiscibleIncompressibleTwoPhaseMixture mixture(U, phi);

volScalarField& alpha1 = mixture.alpha1();
volScalarField& alpha2 = mixture.alpha2();

const dimensionedScalar& rho1 = mixture.rho1();
const dimensionedScalar& rho2 = mixture.rho2();

// Effective mixture density field for ddt
volScalarField rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT
    ),
    alpha1*rho1 + alpha2*rho2
);
rho.oldTime();

// Mass flux
surfaceScalarField rhoPhi
(
    IOobject
    (
        "rhoPhi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    fvc::interpolate(rho)*phi
);

// Turbulence / interphase transport
typedef incompressibleInterPhaseTransportModel<
    immiscibleIncompressibleTwoPhaseMixture
> transportModelType;

autoPtr<transportModelType> turbulence
(
    new transportModelType(rho, U, phi, rhoPhi, mixture)
);

// Compute kinematic viscosity and dynamic viscosity
volScalarField nu = mixture.nu();
volScalarField mu = rho*nu;
Info<< "Computed kinematic viscosity nu and dynamic viscosity mu\n" << endl;

// Compute inverse permeability for Darcy-Brinkman term
Info<< "Computing inverse permeability invK = mu / K\n" << endl;
volScalarField invK
(
    IOobject
    (
        "invK",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mu / Kdim
);
invK.write();

#include "readGravitationalAcceleration.H"
#include "readhRef.H"
#include "gh.H"

volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    p_rgh + rho*gh
);

label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell(p, p_rgh, pimple.dict(), pRefCell, pRefValue);

if (p_rgh.needReference())
{
    p += dimensionedScalar("p", p.dimensions(), pRefValue - getRefCellValue(p, pRefCell));
    p_rgh = p - rho*gh;
}

mesh.setFluxRequired(p_rgh.name());
mesh.setFluxRequired(alpha1.name());

#include "createMRF.H"
#include "createFvOptions.H"
