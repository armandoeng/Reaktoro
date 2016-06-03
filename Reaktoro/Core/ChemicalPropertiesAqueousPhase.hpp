// Reaktoro is a unified framework for modeling chemically reactive systems.
//
// Copyright (C) 2014-2015 Allan Leal
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

#pragma once

// C++ includes
#include <memory>

// Reaktoro includes
#include <Reaktoro/Common/ChemicalScalar.hpp>
#include <Reaktoro/Common/ChemicalVector.hpp>

namespace Reaktoro {

// Forward declarations
class ChemicalProperties;
class ChemicalSystem;

/// A class for querying aqueous thermodynamic and chemical properties in a chemical system.
class ChemicalPropertiesAqueousPhase
{
public:
    /// Construct a custom ChemicalPropertiesAqueousPhase instance with given ChemicalProperties.
    explicit ChemicalPropertiesAqueousPhase(const ChemicalProperties& properties);

    /// Return the ionic strength of the aqueous phase.
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto ionicStrength() const -> ChemicalScalar;

    /// Return the pH of the system.
    /// The aqueous phase must have a hydron species named either H+, H+(aq), or H[+].
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto pH() const -> ChemicalScalar;

    /// Return the pE of the system.
    /// This methods calculates pE using the dual chemical potential of charge element.
    /// This is an alternative approach to using a half reaction (Kulik, 2006).
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto pE() const -> ChemicalScalar;

    /// Return the pE of the system calculated using a given half reaction.
    /// Use this method to specify a half reaction for the calculation of pE.
    /// For example:
    /// ~~~
    /// ChemicalProperties properties(system);
    /// properties.update(T, P, n);
    /// properties.aqueous().pE("Fe++ = Fe+++ + e-");
    /// properties.aqueous().pE("0.5*O2(aq) + 2*H+ + 2*e- = H2O(l)");
    /// ~~~
    /// Note that the electro species `e-` must be present in the half reaction.
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto pE(std::string reaction) const -> ChemicalScalar;

    /// Return the reduction potential of the system (in units of V).
    /// This methods calculates Eh using the dual chemical potential of charge element.
    /// This is an alternative approach to using a half reaction (Kulik, 2006).
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto Eh() const -> ChemicalScalar;

    /// Return the reduction  potential of the system calculated using a given half reaction (in units of V).
    /// Use this method to specify a half reaction for the calculation of Eh.
    /// For example:
    /// ~~~
    /// ChemicalProperties properties(system);
    /// properties.update(T, P, n);
    /// properties.aqueous().Eh("Fe++ = Fe+++ + e-");
    /// properties.aqueous().Eh("0.5*O2(aq) + 2*H+ + 2*e- = H2O(l)");
    /// ~~~
    /// Note that the electro species `e-` must be present in the half reaction.
    /// If the chemical system has no aqueous phase, then zero is returned.
    auto Eh(std::string reaction) const -> ChemicalScalar;

private:
    struct Impl;

    std::shared_ptr<Impl> pimpl;
};

} // namespace Reaktoro