/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::immersedBoundaryFvPatchField

Description
    Foam::immersedBoundaryFvPatchField

Author
    Hrvoje Jasak

SourceFiles
    immersedBoundaryFvPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef immersedBoundaryFvPatchField_H
#define immersedBoundaryFvPatchField_H

#include "fvPatchField.H"
#include "immersedBoundaryFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class immersedBoundaryFvPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class immersedBoundaryFvPatchField
:
    public fvPatchField<Type>
{
    // Private data

        //- Local reference cast into the processor patch
        const immersedBoundaryFvPatch& ibPatch_;

        //- Local reference to fvMesh
        const fvMesh& mesh_;

        // Defining fields
        // Note: defining fields carry values on faces of the IB patch
        // represented as a triangulated surface

            //- Defining value field
            Field<Type> refValue_;

            //- Defining normal gradient field
            Field<Type> refGrad_;


        //- Does the boundary condition fix the value
        Switch fixesValue_;


        // Dead cell controls

            //- Set dead cell value
            Switch setDeadCellValue_;

            //- Dead cell value
            Type deadCellValue_;


        // Fields on the IB intersection points
        // Field size equals the number of cells intesected with the IB

            //- Value field
            mutable Field<Type> ibValue_;

            //- Normal gradient field
            mutable Field<Type> ibGrad_;


    // Private Member Functions

        //- Update IB value and gradient
        void updateIbValues() const;

        //- Impose Dirichlet BC at IB cells and return corrected cells values
        //  Calculate value and gradient on IB intersection points
        tmp<Field<Type> > imposeDirichletCondition() const;

        //- Impose Neumann BC at IB cells and return corrected cells values
        //  Calculate value and gradient on IB intersection points
        tmp<Field<Type> > imposeNeumannCondition() const;

        //- Impose condition in dead cells
        void imposeDeadCondition();


        // Manipulate matrix

            //- Check and correct zero diagonal in dead cells
            void correctDiag
            (
                fvMatrix<Type>& eqn
            ) const;

            //- Impose fixed gradient condition by manipulating matrix
            //  Note: reconsider pre-factor for diffusivity
            //  HJ, 16/Apr/2012
            void correctOffDiag
            (
                fvMatrix<Type>& eqn
            ) const;


public:

    //- Runtime type information
    TypeName(immersedBoundaryFvPatch::typeName_());


    // Static algorithm control data

        //- Number of boundary condition update iterations
        static label nBcIter_;

        //- Boundary condition update tolerance
        static scalar bcTolerance_;


    // Constructors

        //- Construct from patch and internal field
        immersedBoundaryFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        immersedBoundaryFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given immersedBoundaryFvPatchField
        //  onto a new patch
        immersedBoundaryFvPatchField
        (
            const immersedBoundaryFvPatchField<Type>&,
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        immersedBoundaryFvPatchField
        (
            const immersedBoundaryFvPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
            (
                new immersedBoundaryFvPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        immersedBoundaryFvPatchField
        (
            const immersedBoundaryFvPatchField<Type>&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchField<Type> > clone
        (
            const DimensionedField<Type, volMesh>& iF
        ) const
        {
            return tmp<fvPatchField<Type> >
            (
                new immersedBoundaryFvPatchField<Type>(*this, iF)
            );
        }


    // Member functions

        //- Return reference to immersed boundary patch
        const immersedBoundaryFvPatch& ibPatch() const
        {
            return ibPatch_;
        }


        // Return defining fields
        // Note: defining fields carry values on faces of the IB patch
        // represented as a triangulated surface

            //- Return access to reference value
            virtual Field<Type>& refValue()
            {
                return refValue_;
            }

            //- Return reference value
            virtual const Field<Type>& refValue() const
            {
                return refValue_;
            }

            //- Return access to reference gradient
            virtual Field<Type>& refGrad()
            {
                return refGrad_;
            }

            //- Return reference gradient
            virtual const Field<Type>& refGrad() const
            {
                return refGrad_;
            }

            //- Return true if this patch field fixes a value
            virtual bool fixesValue() const
            {
                return fixesValue_;
            }

            //- Return access to fixes value switch
            Switch& fixesValue()
            {
                return fixesValue_;
            }


        //- Return fields on intersection points interacting with the IB

            //- Return IB field
            const Field<Type>& ibValue() const;

            //- Return IB surface-normal gradient
            const Field<Type>& ibGrad() const;


        //- Return fields on triangular faces

            //- Return IB field
            tmp<Field<Type> > triValue() const;

            //- Return IB surface-normal gradient
            tmp<Field<Type> > triGrad() const;


        // Mapping functions

            //- Map (and resize as needed) from self given a mapping object
            virtual void autoMap
            (
                const fvPatchFieldMapper&
            )
            {}

            //- Reverse map the given fvPatchField onto this fvPatchField
            virtual void rmap
            (
                const fvPatchField<Type>&,
                const labelList&
            )
            {}


        // Evaluation functions

            //- Update the coefficients associated with the patch field
            void updateCoeffs();

            //- Initialise the evaluation of the patch field
            virtual void initEvaluate
            (
                const Pstream::commsTypes commsType = Pstream::blocking
            );

            //- Evaluate the patch field
            virtual void evaluate
            (
                const Pstream::commsTypes commsType = Pstream::blocking
            );

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type> > valueInternalCoeffs
            (
                const tmp<scalarField>&
            ) const
            {
                return tmp<Field<Type> >(new Field<Type>(0));
            }

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type> > valueBoundaryCoeffs
            (
                const tmp<scalarField>&
            ) const
            {
                return tmp<Field<Type> >(new Field<Type>(0));
            }

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            tmp<Field<Type> > gradientInternalCoeffs() const
            {
                return tmp<Field<Type> >(new Field<Type>(0));
            }

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            tmp<Field<Type> > gradientBoundaryCoeffs() const
            {
                return tmp<Field<Type> >(new Field<Type>(0));
            }

            //- Manipulate matrix
            virtual void manipulateMatrix(fvMatrix<Type>& matrix);


        // I-O

            //- Write
            virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "immersedBoundaryFvPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
