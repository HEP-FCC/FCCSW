/** @class ParticleStatus
 *
 *  Lists possible statuses of simulated or reconstructed particles
 *
 *  @author: Z. Drasal (CERN)
 *
 */
#ifndef INCLUDE_PARTICLE_STATUS_H_
#define INCLUDE_PARTICLE_STATUS_H_

namespace ParticleStatus {

  // Particle bits related to status
  static const int Beam           = 0x000;
  static const int Decayed        = 0x001;
  static const int Stable         = 0x002;
  static const int Unmatched      = 0x003;
  static const int MatchInCascade = 0x004;

} // Namespace

#endif // INCLUDE_PARTICLE_STATUS_H_
