# This directory is for smart components, which are Components that:
**-**: Use services (from data-access dir)
**-**: Use dumb components (from ui dir)
**-**: Use utils (from utils dir)
**-**: Use utils (from utils dir)
**-**: Complex logic
**-**: Dependency injection
**-**: Setting up streams
**-**: Other controller type behaviour
**-**: "Know" things about the apps structure

# This directory also holds our routed components for a particular feature
**-**: That is a component that is activated by going to a particular route
**-**: The sub dirs in here would be child routes (e.g. Parent route Ships, child route Ships/:id)

# This directory also holds our "shell" router for the feature
**-**: e.g. ships/ship-shell/ship-shell-routes.ts

## How to structure your Angular apps like a Googler
https://www.youtube.com/watch?v=7SDpTOLeqHE