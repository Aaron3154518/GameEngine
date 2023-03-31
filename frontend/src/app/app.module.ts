import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { MdbModalModule } from 'mdb-angular-ui-kit/modal';
import { FormsModule } from '@angular/forms';

import {
  FaIconLibrary,
  FontAwesomeModule,
} from '@fortawesome/angular-fontawesome';
import { far } from '@fortawesome/free-regular-svg-icons';
import { fas } from '@fortawesome/free-solid-svg-icons';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { ParametersComponent } from './parameters/parameters.component';
import { ParameterComponent } from './parameter/parameter.component';
import { TypeModalComponent } from './type-modal/type-modal.component';
import {
  ParameterGroupSearchComponent,
  VarComponent,
} from './parameter-group-search/parameter-group-search.component';
import { OnInitDirective } from './directives/on-init.directive';
import {
  GroupComponent,
  ParameterGroupPipe,
  ParametersSearchComponent,
} from './parameters-search/parameters-search.component';
import { SearchComponent, ColHeaderComponent } from './search/search.component';
import { InputComponent } from './search/input/input.component';
import { ContainerDirective } from './directives/container.directive';

@NgModule({
  declarations: [
    AppComponent,
    ParametersComponent,
    ParameterComponent,
    TypeModalComponent,
    ParameterGroupSearchComponent,
    OnInitDirective,
    ParametersSearchComponent,
    SearchComponent,
    InputComponent,
    ContainerDirective,
    VarComponent,
    GroupComponent,
    ColHeaderComponent,
    ParameterGroupPipe,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    FontAwesomeModule,
    MdbModalModule,
    FormsModule,
  ],
  providers: [],
  bootstrap: [AppComponent],
})
export class AppModule {
  constructor(library: FaIconLibrary) {
    // Add multiple icons to the library
    library.addIconPacks(far, fas);
  }
}
