// Reaktor is a C++ library for computational reaction modelling.
//
// Copyright (C) 2014 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "Gems.hpp"

// Gems includes
#define IPMGEMPLUGIN
#define NOPARTICLEARRAY
#include <Reaktor/gems3k/node.h>

// Reaktor includes
#include <Reaktor/Common/TimeUtils.hpp>
#include <Reaktor/Core/ChemicalSystem.hpp>
#include <Reaktor/Core/ChemicalState.hpp>

namespace Reaktor {

struct Gems::Impl
{
    /// The TNode instance from Gems
    TNode node;

    /// The elapsed time of the equilibrate method (in units of s)
    double elapsed_time = 0;
};

Gems::Gems()
: pimpl(new Impl())
{}

Gems::Gems(std::string filename)
: pimpl(new Impl())
{
    if(node().GEM_init(filename.c_str()))
        throw std::runtime_error("Error reading the Gems chemical system specification file.");
}

auto Gems::setTemperature(double val) -> void
{
    node().setTemperature(val);
}

auto Gems::setPressure(double val) -> void
{
    node().setPressure(val);
}

auto Gems::setSpeciesAmounts(const Vector& n) -> void
{
    node().setSpeciation(n.data());
}

auto Gems::setElementAmounts(const Vector& b) -> void
{
    // Set amounts of the elements
    for(unsigned i = 0; i < numElements(); ++i)
        node().Set_IC_b(b[i], i);

    // Set charge to zero
    node().Set_IC_b(0.0, numElements() + 1);
}

auto Gems::numElements() const -> unsigned
{
    return node().pCSD()->nIC - 1;
}

auto Gems::numSpecies() const -> unsigned
{
    return node().pCSD()->nDC;
}

auto Gems::numPhases() const -> unsigned
{
    return node().pCSD()->nPH;
}

auto Gems::numSpeciesInPhase(unsigned index) const -> unsigned
{
    return node().pCSD()->nDCinPH[index];
}

auto Gems::elementName(unsigned index) const -> std::string
{
    return node().pCSD()->ICNL[index];
}

auto Gems::speciesName(unsigned index) const -> std::string
{
    return node().pCSD()->DCNL[index];
}

auto Gems::phaseName(unsigned index) const -> std::string
{
    return node().pCSD()->PHNL[index];
}

auto Gems::elementIndex(std::string name) const -> unsigned
{
    unsigned index = 0;
    const unsigned size = numElements();
    for(; index < size; ++index)
        if(elementName(index) == name)
            return index;
    return size;
}

auto Gems::speciesIndex(std::string name) const -> unsigned
{
    unsigned index = 0;
    const unsigned size = numSpecies();
    for(; index < size; ++index)
        if(speciesName(index) == name)
            return index;
    return size;
}

auto Gems::phaseIndex(std::string name) const -> unsigned
{
    unsigned index = 0;
    const unsigned size = numPhases();
    for(; index < size; ++index)
        if(phaseName(index) == name)
            return index;
    return size;
}

auto Gems::elementAtomsInSpecies(unsigned ielement, unsigned ispecies) const -> double
{
    return node().DCaJI(ispecies, ielement);
}

auto Gems::speciesCharge(unsigned index) const -> double
{
    return elementAtomsInSpecies(numElements(), index);
}

auto Gems::elementsInSpecies(unsigned index) const -> std::map<unsigned, double>
{
    std::map<unsigned, double> elements;
    for(unsigned j = 0; j < numElements(); ++j)
        if(elementAtomsInSpecies(j, index))
            elements[j] = elementAtomsInSpecies(j, index);
    return elements;
}

auto Gems::elementMolarMass(unsigned index) const -> double
{
    return node().ICmm(index);
}

auto Gems::speciesMolarMass(unsigned index) const -> double
{
    return node().DCmm(index);
}

auto Gems::temperature() const -> double
{
    return node().Get_TK();
}

auto Gems::pressure() const -> double
{
    return node().Get_P();
}

auto Gems::elementAmounts() const -> Vector
{
    Vector b(numElements());
    for(unsigned i = 0; i < b.size(); ++i)
        b[i] = node().Get_bIC(i);
    return b;
}

auto Gems::speciesAmounts() const -> Vector
{
    Vector n(numSpecies());
    for(unsigned i = 0; i < n.size(); ++i)
        n[i] = speciesAmount(i);
    return n;
}

auto Gems::speciesAmount(unsigned index) const -> double
{
    return node().Get_nDC(index);
}

auto Gems::speciesAmountsInPhase(unsigned index) const -> Vector
{
    const unsigned size = numSpeciesInPhase(index);
    Vector np(size);
    unsigned offset = 0;
    for(unsigned i = 0; i < index; ++i)
        offset += numSpeciesInPhase(i);
    for(unsigned i = 0; i < size; ++i)
        np[i] = speciesAmount(offset + i);
    return np;
}

auto Gems::formulaMatrix() const -> Matrix
{
    const unsigned E = numElements();
    const unsigned N = numSpecies();
    Matrix A(E, N);
    for(unsigned i = 0; i < N; ++i)
        for(unsigned j = 0; j < E; ++j)
            A(j, i) = elementAtomsInSpecies(j, i);
    return A;
}

auto Gems::gibbsEnergies() -> Vector
{
    const unsigned num_species = numSpecies();
    Vector u0(num_species);
    node().updateStandardGibbsEnergies();
    ACTIVITY* ap = node().pActiv()->GetActivityDataPtr();
    for(unsigned i = 0; i < num_species; ++i)
        u0[i] = ap->tpp_G[i];
    return u0;
}

auto Gems::chemicalPotentials() -> Vector
{
    const double T = temperature();
    const double R = 8.31451; // The universal gas constant used in GEMS
    const double RT = R*T;
    const unsigned num_species = numSpecies();
    Vector u(num_species);
    node().updateStandardGibbsEnergies();
    node().initActivityCoefficients();
    node().updateConcentrations();
    node().updateActivityCoefficients();
    node().updateChemicalPotentials();
    ACTIVITY* ap = node().pActiv()->GetActivityDataPtr();
    for(unsigned i = 0; i < num_species; ++i)
        u[i] = RT*ap->F[i]; // RT factor to scale back to J/mol
    return u;
}

auto Gems::equilibrate() -> void
{
    Time start = time();
    node().pCNode()->NodeStatusCH = NEED_GEM_SIA;
    node().GEM_run(false);
    pimpl->elapsed_time = elapsed(start);
}

auto Gems::converged() const -> bool
{
    const auto status = node().pCNode()->NodeStatusCH;
    return status == OK_GEM_AIA or status == OK_GEM_SIA;
}

auto Gems::numIterations() const -> unsigned
{
    return node().pCNode()->IterDone;
}

auto Gems::elapsedTime() const -> double
{
    return pimpl->elapsed_time;
}

auto Gems::node() -> TNode&
{
    return pimpl->node;
}

auto Gems::node() const -> const TNode&
{
    return pimpl->node;
}

namespace helper {

auto createElement(const Gems& gems, unsigned ielement) -> Element
{
    ElementData data;
    data.name = gems.elementName(ielement);
    data.molar_mass = gems.elementMolarMass(ielement);
    return Element(data);
}

auto createSpecies(const Gems& gems, unsigned ispecies) -> Species
{
    SpeciesData data;
    data.name = gems.speciesName(ispecies);
    data.molar_mass = gems.speciesMolarMass(ispecies);
    data.charge = gems.speciesCharge(ispecies);
    data.formula = data.name;
    for(auto pair : gems.elementsInSpecies(ispecies))
    {
        data.elements.push_back(createElement(gems, pair.first));
        data.atoms.push_back(pair.second);
    }
    return Species(data);
}

auto createPhase(const Gems& gems, unsigned iphase) -> Phase
{
    PhaseData data;
    data.name = gems.phaseName(iphase);
    for(unsigned ispecies = 0; ispecies < gems.numSpecies(); ++ispecies)
        data.species.push_back(createSpecies(gems, ispecies));
    return Phase(data);
}

auto createPhases(const Gems& gems) -> std::vector<Phase>
{
    std::vector<Phase> phases;
    unsigned offset = 0;
    for(unsigned iphase = 0; iphase < gems.numPhases(); ++iphase)
    {
        PhaseData data;
        data.name = gems.phaseName(iphase);
        for(unsigned ispecies = offset; ispecies < offset + gems.numSpeciesInPhase(iphase); ++ispecies)
            data.species.push_back(createSpecies(gems, ispecies));
        phases.push_back(Phase(data));
        offset += gems.numSpeciesInPhase(iphase);
    }
    return phases;
}

} // namespace helper

Gems::operator ChemicalSystem() const
{
    Gems gems = *this;

    const unsigned num_species = gems.numSpecies();

    const Vector zero_vec = zeros(num_species);
    const Matrix zero_mat = zeros(num_species, num_species);

    ChemicalSystemData data;

    data.phases = helper::createPhases(*this);

    data.gibbs_energies = [=](double T, double P) mutable -> ThermoVector
    {
        gems.setTemperature(T);
        gems.setPressure(P);
        return ThermoVector(gems.gibbsEnergies(), zero_vec, zero_vec);
    };

    data.chemical_potentials = [=](double T, double P, const Vector& n) mutable -> ChemicalVector
    {
        gems.setTemperature(T);
        gems.setPressure(P);
        gems.setSpeciesAmounts(n);
        return ChemicalVector(gems.chemicalPotentials(), zero_vec, zero_vec, zero_mat);
    };

    return ChemicalSystem(data);
}

Gems::operator ChemicalState() const
{
    ChemicalSystem system = *this;
    ChemicalState state(system);
    state.setTemperature(temperature());
    state.setPressure(pressure());
    state.setSpeciesAmounts(speciesAmounts());
    return state;
}

} // namespace Reaktor
