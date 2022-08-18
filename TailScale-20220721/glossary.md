# Tailscale Glossary

**Coordination Server**:

A machine on the Internet that coordinates and configures your
_Nodes_ for you.

A Coordination Server is a service provided by _Tailscale_ or
can be self-hosted by _Headscale_.

**DERP (Designated Encrypted Relay for Packets)**:

A server that acts as a Relay between two _Nodes_ that can't
communicate directly because of firewall restrictions or NATs.
Uses HTTPS to tunnel.

DERPs are provided for free by _Tailscale_ or you can also
register your own DERPs for your _Tailnet_ to reduce latency.

**Exit Node**:

A _Node_ that provides routing outside of its _Tailnet_ to
the Internet.

**Headscale**:

A free and open source, 3rd party software package that
can be used to self-host your own _Coordination Server_.
See https://github.com/juanfont/headscale.

**Hub and Spoke Network**:

A network where machines route all their network traffic through a
central server (_Relay_).

**Mesh Network**:

A network where the machines communicate directly with each other
without passing through a _Relay_.

**Node**:

A machine registered in a private tailscale network (_Tailnet_).

**Relay**:

A server that acts as an invisible intermediary between two
other machines to route their packets to each other.

**Shared Node**:

A _Node_ that appears in one or more _Tailnets_ simultaneously.

**Subnet Router**:

A _Node_ that provides routing to a local network, so machines that
are not participating in its _Tailnet_ can be accessed.

**Taildrop**:

_Tailscale_ feature that allows copying files between devices on
your _Tailnet_.

**Tailnet**:

A set of machines (_Nodes_) connected together in a private
_Tailscale_ network.

**Tailscale**:

A software product that allows you to connect machines together
anywhere with Internet access in a private _Mesh Network_.

Also, the company that makes the _Tailscale_ product, see
https://tailscale.com/

See _Headscale_.

**Tailscale IP Address**:

A unique IP address assigned to each _Node_ on your _Tailnet_.
They are all in the range 100.x.y.z (e.g. 100.10.25.30).
