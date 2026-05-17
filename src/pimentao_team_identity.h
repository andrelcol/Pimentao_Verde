// -*-c++-*-
/*!
  \file pimentao_team_identity.h
  \brief Nome da equipa no servidor (logs .rcg / monitor) vs. identidade interna do código.

  O repositório e configs JSON continuam em "pimentao_verde"; em competição e logs
  usa-se normalmente \c Rinobot-Team. Ambos activam a mesma lógica personalizada.
 */

#ifndef PIMENTAO_TEAM_IDENTITY_H
#define PIMENTAO_TEAM_IDENTITY_H

#include <string>

inline bool
is_our_pimentao_team( const std::string & team_name )
{
    return team_name == "Pimentao_Verde"
        || team_name == "Rinobot-Team";
}

#endif
