---
description: "Request human review after the agent has reviewed a pull request."
on:
  pull_request_target:
    types: [labeled, ready_for_review]
  roles: [read, triage, write, maintain, admin]
  status-comment: true
if: "github.event.pull_request.draft == false && (github.event.action == 'ready_for_review' || github.event.label.name == format('Review{0} Agent Reviewed', ':'))"

checkout: false

permissions:
  contents: read
  pull-requests: read
  copilot-requests: write

network: defaults

tools:
  github:
    toolsets: [pull_requests]

safe-outputs:
  add-reviewer:
    allowed-reviewers: [carsonradtke]
    target: triggering
    required-labels: ["Review: Agent Reviewed"]
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-labels:
    allowed: ["Status: Under Review"]
    required-labels: ["Review: Agent Reviewed"]
    max: 1
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  remove-labels:
    allowed:
      - "Status: Open"
      - "Status: RCA Complete"
      - "Status: Fix In Progress"
    required-labels: ["Review: Agent Reviewed"]
    max: 3
    github-token: ${{ secrets.GH_AW_AGENT_TOKEN }}
  add-comment:
    required-labels: ["Review: Agent Reviewed"]
    max: 1

timeout-minutes: 10
max-ai-credits: -1
max-daily-ai-credits: -1
---

# Request human review

Confirm that the triggering pull request:

- Has `Review: Agent Reviewed`.
- Is not a draft.
- Does not have `Agent: Paused`.

If any prerequisite is missing, do not request a reviewer or change status. Otherwise request
`@carsonradtke` as a reviewer, replace any prior workflow status with `Status: Under Review`, and
comment that automated classification and review are complete.

Do not approve or merge the pull request.
