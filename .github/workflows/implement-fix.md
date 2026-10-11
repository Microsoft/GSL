---
description: "Assign the coding agent to an RCA-complete pull request."
on:
  pull_request:
    types: [labeled]
  status-comment: true
if: "github.event.label.name == format('Status{0} RCA Complete', ':')"

permissions:
  contents: read
  issues: read
  pull-requests: read
  copilot-requests: write

network: defaults

tools:
  github:
    toolsets: [default]

safe-outputs:
  assign-to-agent:
    name: copilot
    custom-agent: coding-agent
    allowed: [copilot]
    target: triggering
    base-branch: main
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-labels:
    allowed: ["Status: Fix In Progress"]
    required-labels: ["Status: RCA Complete"]
    required-title-prefix: "[RCA] "
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  remove-labels:
    allowed: ["Status: RCA Complete"]
    required-labels: ["Status: RCA Complete"]
    required-title-prefix: "[RCA] "
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-comment:
    required-labels: ["Status: RCA Complete"]
    required-title-prefix: "[RCA] "
    max: 1

timeout-minutes: 10
max-ai-credits: -1
max-daily-ai-credits: -1
---

# Hand the RCA pull request to the coding agent

Confirm that the triggering pull request is a draft, has `Status: RCA Complete`, starts with
`[RCA]`, contains a linked issue, an RCA summary, and a failing regression test, and does not have
`Agent: Paused`.

If any prerequisite is missing, comment with the missing prerequisite and do not assign an agent.
Otherwise assign the `coding-agent` custom agent to the triggering pull request, add
`Status: Fix In Progress`, remove `Status: RCA Complete`, and comment that implementation has
started.

The coding agent must work on this existing pull request. It must not open a separate replacement
pull request, approve the pull request, or merge it.
