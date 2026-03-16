/**
 * Procedural sound effects using Web Audio API.
 * No external audio files required.
 */
export class AquilaSoundService {
  private static ctx: AudioContext | null = null;
  private static resumeListenerAdded = false;

  private static getContext(): AudioContext {
    if (!AquilaSoundService.ctx) {
      AquilaSoundService.ctx = new AudioContext();
    }
    // Browsers suspend AudioContext until user interaction.
    // Add a one-time listener to resume on first click/key.
    if (!AquilaSoundService.resumeListenerAdded) {
      AquilaSoundService.resumeListenerAdded = true;
      const tryResume = () => {
        const c = AquilaSoundService.ctx;
        if (c && c.state === 'suspended') {
          c.resume();
        }
        document.removeEventListener('pointerdown', tryResume);
        document.removeEventListener('keydown', tryResume);
      };
      document.addEventListener('pointerdown', tryResume, { once: true });
      document.addEventListener('keydown', tryResume, { once: true });
    }
    return AquilaSoundService.ctx;
  }

  /** Short laser/rocket fire sound */
  static playFireSound(volume: number = 0.15) {
    const ctx = this.getContext();
    const now = ctx.currentTime;

    const osc = ctx.createOscillator();
    const gain = ctx.createGain();
    osc.connect(gain);
    gain.connect(ctx.destination);

    osc.type = 'sawtooth';
    osc.frequency.setValueAtTime(800, now);
    osc.frequency.exponentialRampToValueAtTime(200, now + 0.15);

    gain.gain.setValueAtTime(volume, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.15);

    osc.start(now);
    osc.stop(now + 0.15);
  }

  /** Enemy fire sound — slightly different pitch */
  static playEnemyFireSound(volume: number = 0.1) {
    const ctx = this.getContext();
    const now = ctx.currentTime;

    const osc = ctx.createOscillator();
    const gain = ctx.createGain();
    osc.connect(gain);
    gain.connect(ctx.destination);

    osc.type = 'square';
    osc.frequency.setValueAtTime(600, now);
    osc.frequency.exponentialRampToValueAtTime(150, now + 0.12);

    gain.gain.setValueAtTime(volume, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.12);

    osc.start(now);
    osc.stop(now + 0.12);
  }

  /** Projectile impact/explosion — quick burst of noise */
  static playProjectileExplodeSound(volume: number = 0.15) {
    const ctx = this.getContext();
    const now = ctx.currentTime;
    const duration = 0.25;

    // White noise burst
    const bufferSize = ctx.sampleRate * duration;
    const buffer = ctx.createBuffer(1, bufferSize, ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < bufferSize; i++) {
      data[i] = (Math.random() * 2 - 1);
    }

    const noise = ctx.createBufferSource();
    noise.buffer = buffer;

    const bandpass = ctx.createBiquadFilter();
    bandpass.type = 'bandpass';
    bandpass.frequency.setValueAtTime(1000, now);
    bandpass.Q.setValueAtTime(0.5, now);

    const gain = ctx.createGain();
    gain.gain.setValueAtTime(volume, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + duration);

    noise.connect(bandpass);
    bandpass.connect(gain);
    gain.connect(ctx.destination);

    noise.start(now);
    noise.stop(now + duration);
  }

  /** Ship explosion — deeper, longer boom */
  static playShipExplodeSound(volume: number = 0.25) {
    const ctx = this.getContext();
    const now = ctx.currentTime;
    const duration = 0.6;

    // Low rumble oscillator
    const osc = ctx.createOscillator();
    const oscGain = ctx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(120, now);
    osc.frequency.exponentialRampToValueAtTime(30, now + duration);
    oscGain.gain.setValueAtTime(volume, now);
    oscGain.gain.exponentialRampToValueAtTime(0.001, now + duration);
    osc.connect(oscGain);
    oscGain.connect(ctx.destination);
    osc.start(now);
    osc.stop(now + duration);

    // Noise layer for debris/crackle
    const bufferSize = ctx.sampleRate * duration;
    const buffer = ctx.createBuffer(1, bufferSize, ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < bufferSize; i++) {
      data[i] = (Math.random() * 2 - 1);
    }
    const noise = ctx.createBufferSource();
    noise.buffer = buffer;

    const lowpass = ctx.createBiquadFilter();
    lowpass.type = 'lowpass';
    lowpass.frequency.setValueAtTime(800, now);
    lowpass.frequency.exponentialRampToValueAtTime(100, now + duration);

    const noiseGain = ctx.createGain();
    noiseGain.gain.setValueAtTime(volume * 0.6, now);
    noiseGain.gain.exponentialRampToValueAtTime(0.001, now + duration);

    noise.connect(lowpass);
    lowpass.connect(noiseGain);
    noiseGain.connect(ctx.destination);
    noise.start(now);
    noise.stop(now + duration);
  }

  /**
   * Creates a persistent engine sound that can be volume-controlled.
   * Returns an object with setIntensity(0-1) and stop() methods.
   */
  static createEngineSound(baseFreq: number = 55, maxVolume: number = 0.12): { setIntensity: (t: number) => void; stop: () => void } {
    const ctx = this.getContext();
    if (ctx.state === 'suspended') {
      ctx.resume();
    }

    // Low rumble oscillator
    const osc = ctx.createOscillator();
    osc.type = 'sawtooth';
    osc.frequency.value = baseFreq;

    // Second harmonic for richness
    const osc2 = ctx.createOscillator();
    osc2.type = 'triangle';
    osc2.frequency.value = baseFreq * 2;

    const gain = ctx.createGain();
    gain.gain.value = maxVolume * 0.2; // start quiet (idle)

    // Lowpass to make it feel like a muffled engine
    const lowpass = ctx.createBiquadFilter();
    lowpass.type = 'lowpass';
    lowpass.frequency.value = 200;

    osc.connect(lowpass);
    osc2.connect(lowpass);
    lowpass.connect(gain);
    gain.connect(ctx.destination);

    osc.start();
    osc2.start();

    return {
      /** t: 0 = idle, 1 = full thrust */
      setIntensity(t: number) {
        const clamped = Math.max(0, Math.min(1, t));
        const vol = maxVolume * (0.15 + 0.85 * clamped);
        gain.gain.setTargetAtTime(vol, ctx.currentTime, 0.1);
        osc.frequency.setTargetAtTime(baseFreq + clamped * 20, ctx.currentTime, 0.1);
        osc2.frequency.setTargetAtTime((baseFreq + clamped * 20) * 2, ctx.currentTime, 0.1);
        lowpass.frequency.setTargetAtTime(200 + clamped * 200, ctx.currentTime, 0.1);
      },
      stop() {
        gain.gain.setTargetAtTime(0, ctx.currentTime, 0.05);
        setTimeout(() => {
          osc.stop();
          osc2.stop();
          osc.disconnect();
          osc2.disconnect();
          lowpass.disconnect();
          gain.disconnect();
        }, 200);
      },
    };
  }
}
