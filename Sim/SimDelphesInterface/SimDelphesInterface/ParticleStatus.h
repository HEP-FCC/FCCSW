/** @class ParticleStatus
 *
 *  Lists possible statuses of simulated or reconstructed particles
 *
 *  @author: Z. Drasal (CERN)
 *
 */
#ifndef INCLUDE_PARTICLE_STATUS_H
#define INCLUDE_PARTICLE_STATUS_H

enum class ParticleStatus : unsigned {
  kBeam = 0x000,
  kStable = 0x001,
  kDecayed = 0x002,
  kMatched = 0x003,
  kUnmatched = 0x004,
  kMatchInCascade = 0x005
};  // Enum

#endif  // INCLUDE_PARTICLE_STATUS_H
