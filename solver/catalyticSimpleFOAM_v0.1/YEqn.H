tmp<fv::convectionScheme<scalar> > mvConvection
(
    fv::convectionScheme<scalar>::New
    (
        mesh,
        fields,
        phi,
        mesh.divScheme("div(phi,Yi_h)")
    )
);


{
    reaction->correct();
    #include "chemistry.H"
    
    label inertIndex = -1;
    volScalarField Yt(0.0*Y[0]);

    forAll(Y, i)
    {
        if (Y[i].name() != inertSpecie)
        {
	    volScalarField& Yi = Y[i];
	    volScalarField& Dmixi = Dmix_[i];

	    fvScalarMatrix YiEqn
	    (
		mvConvection->fvmDiv(phi, Yi)
	      - fvm::laplacian(turbulence->mut()+rho*Dmixi, Yi)
	     ==
	      	//R_[i] +
		fvOptions(rho, Yi)
	    );

	    YiEqn.relax();

	    fvOptions.constrain(YiEqn);

	    YiEqn.solve(mesh.solver("Yi"));

	    fvOptions.correct(Yi);

	    Yi.max(0.0);
            Yt += Yi;
        }
        else
        {
            inertIndex = i;
        }
    }

    Y[inertIndex] = scalar(1.0) - Yt;
    Y[inertIndex].max(0.0);
}

