import { AfterViewInit, Component, ElementRef, ViewChild } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { Edge, Vertex } from '../utils/interfaces';

import * as tauri from '@tauri-apps/api';
import { BehaviorSubject, Observable, Subject } from 'rxjs';

interface VData {
  depth: number;
  width: number;
  cx: number;
  cy: number;
}

@Component({
  selector: 'app-parameter-visualizer',
  templateUrl: './parameter-visualizer.component.html',
  styleUrls: ['./parameter-visualizer.component.css'],
})
export class ParameterVisualizerComponent implements AfterViewInit {
  edges: Edge[] = [];
  vertices: Vertex[] = [];
  isolated: Vertex[] = [];
  connected: Vertex[] = [];
  dragging?: Vertex;

  cont: BehaviorSubject<string> = new BehaviorSubject('');
  obs: Observable<string> = this.cont.asObservable();

  readonly W: number = 1000;
  readonly R: number = 25;
  readonly W_R: number = this.W / 2 / this.R;
  readonly A_W: number = 7;
  readonly A_STR: string = `M0,0 V${this.A_W * 2} L${this.A_W},${this.A_W} Z`;

  @ViewChild('dummy', { static: true }) dummy?: ElementRef<HTMLElement>;

  constructor(private parameterService: ParameterService) {
    [this.vertices, this.edges] = this.parameterService.createGraph();
    this.vertices.forEach(
      (v: Vertex) =>
        (v.data = <VData>{
          depth: -1,
          width: 0,
        })
    );
    this.calcDepths(this.vertices);
    this.vertices.sort((a: Vertex, b: Vertex) => a.data.depth - b.data.depth);

    // Get subarrays
    this.isolated = this.vertices.filter((v: Vertex) => !v.in && !v.out);
    this.connected = this.vertices.filter(
      (v: Vertex) => v.in.length > 0 || v.out.length > 0
    );

    this.connected.forEach((v: Vertex, i: number) => {
      v.data.cx = this.getCX(v, i);
      v.data.cy = this.getCY(v, i);
    });
  }

  ngAfterViewInit(): void {
    if (!this.dummy) {
      return;
    }
    let dummy: SVGTextElement = this.dummy
      .nativeElement as any as SVGTextElement;
    this.vertices.forEach((v: Vertex) => {
      dummy.textContent = v.name;
      return Math.min(dummy.getBBox().width, this.R * 2 - 2);
    });
  }

  calcDepth(v: Vertex): number {
    if (v.data.depth !== -1) {
      return v.data.depth;
    }
    v.data.depth =
      v.in.reduce(
        (max: number, e: Edge) => Math.max(max, this.calcDepth(e.from)),
        0
      ) + 1;
    return v.data.depth;
  }

  calcDepths(vs: Vertex[]) {
    vs.forEach((v: Vertex) => this.calcDepth(v));
  }

  getCX(v: Vertex, i: number): number {
    return (((i % this.W_R) + 0.5) * this.W) / this.W_R;
  }

  getCY(v: Vertex, i: number): number {
    return ((Math.floor(i / this.W_R) + 0.5) * this.W) / this.W_R;
  }

  startDrag(v: Vertex) {
    this.dragging = v;
  }

  drag(event: Event, svg: HTMLElement) {
    if (this.dragging) {
      let e: DragEvent = event as DragEvent;
      this.dragging.data.cx = (e.x * this.W) / svg.clientWidth;
      this.dragging.data.cy = (e.y * this.W) / svg.clientHeight;
    }
  }

  endDrag() {
    this.dragging = undefined;
  }

  async getPopoverContent(v: Vertex) {
    let str: string = await tauri.invoke('greet', { name: 'World' });
    this.cont.next(
      `<span class="code">Parameter Set: ${
        v.set.name
      }${str}</span><br><span class="code">${
        v.group.name !== ' ' ? `Parameter Group: ${v.group.name}\n` : ''
      }</span>`
    );
  }
}
